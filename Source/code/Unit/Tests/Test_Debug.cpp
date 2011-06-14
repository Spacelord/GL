#include "Tests.hpp"
#include "SL.Debug\Utility.hpp"

using namespace SL::Debug;

bool Test_Debug()
{
	Write("Print Test: ");
	Write("This should all be on ");
	WriteLine("one line. ");

	return false;
}