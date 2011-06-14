#include "SL\GLWin32.hpp"
#include "SL.Debug\Utility.hpp"
#include "Tests\Tests.hpp"

using namespace SL::Debug;

int main(int argc, char* argv[])
{
	if(Test_Debug())
		WriteLine("Test_Debug: Failed");
	else
		WriteLine("Test_Debug: Pass");

	if(Test_Window())
		WriteLine("Test_Window: Failed");
	else
		WriteLine("Test_Window: Pass");

	return 0;
}