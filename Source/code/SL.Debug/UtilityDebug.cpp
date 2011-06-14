#include "Utility.hpp"
#include <iostream>
#include <string>

using namespace SL;
using namespace std;

void SL::Debug::Write(const string &message) {
	cout << message;
}

void SL::Debug::Write(const boost::format &message) {
	cout << message;
}

void SL::Debug::WriteLine(const string &message) {
	cout << message << std::endl;
}

void SL::Debug::WriteLine(const boost::format &message) {
	cout << message << std::endl;
}