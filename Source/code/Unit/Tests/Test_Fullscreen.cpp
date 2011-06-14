#include "Tests.hpp"
#include "SL\GLWin32.hpp"
#include "SL.Debug\Utility.hpp"

using namespace SL::GL;
using namespace SL::Debug;

// BUG: Fullscreen not working. Check GLWin32 for more info.
bool Test_Fullscreen()
{
	WriteLine("Create Window Test:");

	GLWin32 win;

	win.WindowName("GL Full Screen Window");

	if(win.WindowCreate(1366, 768, 24, false))
		return true;

	while (win.Running())
    {
		win.ProcessEvents();
        Sleep(0);
    }

	win.WindowDestroy();

	return false;
}