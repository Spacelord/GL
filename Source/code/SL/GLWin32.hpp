#ifndef _GLWIN32_HPP
#define _GLWIN32_HPP
#include <windows.h>
#include <ctime>
#include <string>

namespace SL { namespace GL {
	class GLWin32 {
	public:
		GLWin32() {_Running=false;_WindowedMode=false;}
		GLWin32(HINSTANCE appInstanceHandle);

		bool	WindowCreate(int width, int height, int bpp, bool fullScreen);
		void	WindowDestroy();
		void	ProcessEvents();
		void	Swap() { SwapBuffers(_DeviceContext); }

		// Window Data
		bool				Running();
		float				ElapsedSeconds();
		const std::string&	WindowName();
		void				WindowName(std::string value);

		static LRESULT	CALLBACK WinProcStatic(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT			CALLBACK WinProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	private:
		// Windows
		HWND		_WinHandle;
		HGLRC		_RenderingContext;
		HDC			_DeviceContext;
		RECT		_WinRect;
		WNDCLASSEX	_WinClass;
		HINSTANCE	_AppInstanceHandle;

		// Timing
		float		_LastTick;

		// Window Data
		bool		_WindowedMode;
		bool		_Running;
		bool		_QuitRequested;
		bool		_Quitting;
		std::string	_WindowName;

		// Private Methods
		void		_SetupFullscreenMode(int bpp);
		bool		_SetupWindowClass();
		bool		_SetupWindow();
		bool		_SetupGL3Context();
	};
}	}

#endif