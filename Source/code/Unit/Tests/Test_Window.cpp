#include "Tests.hpp"
#include "SL\GLWin32.hpp"
#include "SL.Debug\Utility.hpp"

using namespace SL::GL;
using namespace SL::Debug;

bool Test_Window()
{
	WriteLine("Create Window Test:");

	GLWin32 win;

	win.WindowName("GL Window");

	if(win.WindowCreate(400, 300, 24, true))
		return true;

	while (win.Running())
    {
		win.ProcessEvents();
        Sleep(0);
    }

	win.WindowDestroy();

	return false;
}