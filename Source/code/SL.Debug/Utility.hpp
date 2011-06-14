#ifndef _UTILITY_HPP
#define _UTILITY_HPP
#include <string>
#include <boost\format.hpp>

typedef boost::format _F;
typedef boost::format _FMT;

// TODO:	There are many useful debug and reporting utilities that could be
//			implemented here.

namespace SL { namespace Debug {
	// Debug Utilities
	void Write(const std::string &message);
	void Write(const boost::format &message);

	void WriteLine(const std::string &message);
	void WriteLine(const boost::format &message);

	// misc utilities
} }
#endif