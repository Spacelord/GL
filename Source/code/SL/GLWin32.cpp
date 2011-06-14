#include <ctime>
#include <iostream>
#include <Windows.h>

// TODO: Find a better spot for this
#define GLEW_STATIC

#include <GL\glew.h>
#include <GL\wglew.h>
#include "GLWin32.hpp"
#include "SL.Debug\Utility.hpp"

using namespace SL::GL;

GLWin32::GLWin32(HINSTANCE appInstanceHandle) :
	_Running(false),
	_AppInstanceHandle(appInstanceHandle),
	_LastTick(0)
{
}

// <summary>
// Entry point to setting up and launching the window
// </summary>
bool GLWin32::WindowCreate(int width, int height, int bpp, bool windowed)
{
	// TODO:	There are a ton of Win32 settings beyond these 4 passed in.
	//			There should be a better and more involved way to change them.

	// Set Properties
	_WindowedMode	= windowed;
	_WinRect.top	= (long)0;
	_WinRect.right	= (long)width;
	_WinRect.bottom	= (long)height;
	_WinRect.left	= (long)0;

	if(_SetupWindowClass()) return true;

	// Change the display mode if fullscreen
	if(!_WindowedMode) _SetupFullscreenMode(bpp);

	// Create a generic window, needed before GL window can be created
	if(_SetupWindow()) return true;

	// Init Glew
	GLenum err = glewInit();
	
	// Make sure everything went okay.
	if (GLEW_OK != err) {
		Debug::WriteLine(_F("Error: %1%") % glewGetErrorString(err));
		return true;
	}

	// Report GL version
	const GLubyte *oglVersion = glGetString(GL_VERSION);
    Debug::WriteLine(_F("This system supports OpenGL Version %1%.") % oglVersion);

	// Now that extensions are setup, delete window and start over picking a real format.
	wglMakeCurrent(NULL, NULL);
    wglDeleteContext(_RenderingContext);
    ReleaseDC(_WinHandle, _DeviceContext);
    DestroyWindow(_WinHandle);

	// Create the GL Window
	if(_SetupGL3Context()) return true;

	_LastTick = GetTickCount() / 1000.0f;

	return false;
}

void GLWin32::WindowDestroy()
{
	if(!_WindowedMode)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(true);
	}
}

void GLWin32::ProcessEvents()
{
	MSG msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT GLWin32::WinProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		_Running = true;
		break;
	case WM_DESTROY:
	case WM_CLOSE:
		wglMakeCurrent(_DeviceContext, NULL);
		wglDeleteContext(_RenderingContext);
		_Running = false;
		PostQuitMessage(0);
		return 0;
		break;
	case WM_SIZE:
		{
			// Handle Resize
			int height = HIWORD(lParam);
			int width = LOWORD(lParam);

			glViewport(0, 0, width, height);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			gluPerspective(45.0f, float(width) / float(height), 1.0f, 100.0f);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
		break;
	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE)
			DestroyWindow(_WinHandle);
		break;
	default:
		break;
	}

	return DefWindowProc(wnd, msg, wParam, lParam);
}

LRESULT CALLBACK GLWin32::WinProcStatic(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GLWin32 *window = NULL;

	if(msg == WM_CREATE)
	{
		// Get the pointer we stored during create
		window = (GLWin32*)((LPCREATESTRUCT)lParam)->lpCreateParams;

		// Associate the window pointer with the window handle for the other events to access
		SetWindowLongPtr(wnd, GWL_USERDATA, (LONG_PTR)window);
	}
	else
	{
		// If this is not a creation event then we should have stored a pointer to the window
		window = (GLWin32*)GetWindowLongPtr(wnd, GWL_USERDATA);

		if(!window)
		{
			return DefWindowProc(wnd, msg, wParam, lParam);
		}
	}

	return window->WinProc(wnd, msg, wParam, lParam);
}

#pragma region Window Data
// TODO:	These should be replaced with a more robust property system.
//			See WindowCreate for more info.

const std::string& GLWin32::WindowName()
{
	return _WindowName;
}

void GLWin32::WindowName(std::string value)
{
	_WindowName = value;
}

bool GLWin32::Running()
{
	return _Running;
}

float GLWin32::ElapsedSeconds()
{
	float currentTime = float(GetTickCount()) / 1000.0f;
	float seconds = float(currentTime - _LastTick);
	_LastTick = currentTime;
	return seconds;
}

#pragma endregion

#pragma region Setup Methods

bool GLWin32::_SetupWindowClass()
{
	// Define the Window Class
	_WinClass.cbSize		= sizeof(WNDCLASSEX);
	_WinClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	_WinClass.lpfnWndProc	= GLWin32::WinProcStatic;
	_WinClass.cbClsExtra	= 0;
	_WinClass.cbWndExtra	= 0;
	_WinClass.hInstance		= _AppInstanceHandle;
	_WinClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	_WinClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	_WinClass.hbrBackground	= NULL;
	_WinClass.lpszMenuName	= NULL;
	_WinClass.lpszClassName	= "GLWin32";
	_WinClass.hIconSm		= LoadIcon(NULL, IDI_WINLOGO);

	// Try to register class and return result
	return !RegisterClassEx(&_WinClass);
}

void GLWin32::_SetupFullscreenMode(int bpp)
{
	DEVMODE	screenSettings;	// device mode
	memset(&screenSettings, 0, sizeof(screenSettings));

	screenSettings.dmSize		= sizeof(screenSettings);
	screenSettings.dmPelsWidth	= _WinRect.right;
	screenSettings.dmPelsHeight	= _WinRect.bottom;
	screenSettings.dmBitsPerPel	= bpp;
	screenSettings.dmFields		= DM_BITSPERPEL |
								  DM_PELSWIDTH  |
								  DM_PELSHEIGHT;

	if(ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		// failed to switch to full screen.
		Debug::WriteLine("Full screen display mode failed.");
		_WindowedMode = true;
	}
}

bool GLWin32::_SetupWindow()
{
	DWORD		extendedStyle;
	DWORD		style;
	PIXELFORMATDESCRIPTOR pixelFormatGeneric;

	// BUG: Full screen is not working.
	//		Might need to move these to later in setup
	if(_WindowedMode)
	{
		extendedStyle	= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		style			= WS_OVERLAPPED | WS_BORDER | WS_SYSMENU;
	}
	else
	{
		extendedStyle	= WS_EX_APPWINDOW;
		style			= WS_POPUP;
		ShowCursor(false);
	}
	
	// Adjust window to true size
	AdjustWindowRectEx(&_WinRect, style, false, extendedStyle);

	_WinHandle = CreateWindowEx(NULL,
								"GLWin32",
								WindowName().c_str(),
								style | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
								0, 0,
								_WinRect.right - _WinRect.left,
								_WinRect.bottom - _WinRect.top,
								NULL,
								NULL,
								_AppInstanceHandle,
								this);

	// Check if window was created
	if(!_WinHandle) return true;

	// Create generic context
	_DeviceContext = GetDC(_WinHandle);
	if(_DeviceContext == NULL) return true;

	// Generic Pixel Format
	SetPixelFormat(_DeviceContext, 1, &pixelFormatGeneric);
	_RenderingContext = wglCreateContext(_DeviceContext);
	wglMakeCurrent(_DeviceContext, _RenderingContext);

	// All good?
	if ((0 == _DeviceContext) ||
        (0 == _RenderingContext))
    {
        Debug::WriteLine("An error occured creating an OpenGL window.");
		return true;
    }

	return false;
}

bool GLWin32::_SetupGL3Context()
{
	int nPixCount = 0;
	int nPixelFormat  = -1;

	if(_SetupWindow()) return true;

    int pxAttribs[] = { WGL_SUPPORT_OPENGL_ARB, 1, // Must support OGL rendering
						WGL_DRAW_TO_WINDOW_ARB, 1, // pf that can run a window
						WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB, // must be HW accelerated
						WGL_RED_BITS_ARB,       8, // 8 bits of red precision in window
						WGL_GREEN_BITS_ARB,     8, // 8 bits of green precision in window
						WGL_BLUE_BITS_ARB,      8, // 8 bits of blue precision in window
						WGL_DEPTH_BITS_ARB,     24, // 24 bits of depth precision for window
						WGL_PIXEL_TYPE_ARB,      WGL_TYPE_RGBA_ARB, // pf should be RGBA type
						0};
	
	// Ask OpenGL to find the most relevant format matching our attribs
    // Only get one format back.
	wglChoosePixelFormatARB(_DeviceContext, &pxAttribs[0], NULL, 1, &nPixelFormat, (UINT*)&nPixCount);

	// Check that a format was found
	if(nPixelFormat == -1) 
	{
		_DeviceContext = 0;
		Debug::WriteLine("An error occurred trying to find a pixel format with the requested attribs.");
		return true;
	}

	// Create a GL 3.3 context
	GLint attribs[] = {WGL_CONTEXT_MAJOR_VERSION_ARB,  3,
	                   WGL_CONTEXT_MINOR_VERSION_ARB,  3,
                       0};

	_RenderingContext = wglCreateContextAttribsARB(_DeviceContext, 0, attribs);

	// TODO: If fails work back each version until you find one
	//		 There should be a property so you can set the minimum supported version.
	//		 Default will work back to 3.0 then fail.

	while(_RenderingContext == NULL)
    {
		if(attribs[3] == 0) {
			Debug::WriteLine("Unable to create a 3.x context.");
			return true;
		}

        Debug::WriteLine(_F("Could not create an OpenGL 3.%1% context.") % attribs[3]);
        attribs[3] -= 1;
        _RenderingContext = wglCreateContextAttribsARB(_DeviceContext, 0, attribs);
    }

	// Make context current
	wglMakeCurrent(_DeviceContext, _RenderingContext);

	// All good again?
	if ((0 == _DeviceContext) ||
        (0 == _RenderingContext))
    {
        Debug::WriteLine("An error occured creating an OpenGL window.");
		return true;
    }

	// Show window
	ShowWindow(_WinHandle, SW_SHOW);
	SetForegroundWindow(_WinHandle);
	SetFocus(_WinHandle);

	return false;
}

#pragma endregion