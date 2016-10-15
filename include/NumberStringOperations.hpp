#pragma once



#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <string>
#include <algorithm>
#include <stdexcept>



namespace NumberStringOperations
{
	std::string									ltosCustom(long num, int base, std::string baseAlphabet);
	long										stolCustom(std::string num, int base, std::string baseAlphabet);
	std::string									print(std::string fmt, ...);
}
