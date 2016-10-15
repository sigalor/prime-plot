#include "NumberStringOperations.hpp"



std::string NumberStringOperations::ltosCustom(long num, int base, std::string baseAlphabet)					//long to string; convert the number 'num' into a number string in the specified base with the specified alphabet. If you want usual base 10, set base=10 and baseAlphabet="0123456789". The function will return the number of characters needed for stringifying the number
{
	std::string ret;

	if(baseAlphabet.size() != (std::size_t)base)
		throw std::logic_error("Length of base alphabet has to equal base");
	if(base < 2)																								//it is impossible to represent a number in a base less than 2, so return an empty string in that case
		throw std::logic_error("Cannot convert to base " + std::to_string(base));
	if(num == 0)																								//if 'num' is zero, that is a special case
	{
		ret.push_back(baseAlphabet[0]);																			//just convert it...
		return ret;																								//and return it directly
	}

	while(num != 0)																								//the loop shall end when num became zero
	{
		//TODO: if Intel processor is used, use 'div' as inline assembly to get quotient and remainder in a single operation
		ret.push_back(baseAlphabet[num%base]);																	//just use modulo and divide repeatedly
		num /= base;
	}

	//TODO: what is faster: use 'ret.insert(ret.begin(), baseAlphabet[num%base])' in the previous while-loop or use the current method (reverse after the loop)
	std::reverse(ret.begin(), ret.end());																		//the method used here fills 'buffer' with the characters in reversed order (e.g. if num==1234, 'ret' would contain "4321")

	return ret;																									//return the number of characters the stringifyed number has/'buffer' keeps
}

long NumberStringOperations::stolCustom(std::string num, int base, std::string baseAlphabet)					//string to long
{
	long ret = 0;
	std::size_t charIdx, charNum = 0;
	
	while(num.size() != 0)
	{
		charIdx = baseAlphabet.find(num.back());
		if(charIdx == std::string::npos)
			throw std::logic_error("Character '" + std::string(1, num.back()) + "' does not match any in alphabet\n");
		if(base == 2  &&  charIdx == 1)																			//conversion to base 2 can be made more efficient by using binary operations
			ret |= (1 << charNum);
		else
			ret += charIdx * pow(base, charNum);
		charNum++;
		num.pop_back();
	}
	
	return ret;
}

std::string NumberStringOperations::print(std::string fmt, ...)
{
	static char buffer[1000] = "";
	va_list argPtr;

	va_start(argPtr, fmt);
	vsnprintf(buffer, 1000, fmt.c_str(), argPtr);
	va_end(argPtr);

	return std::string(buffer);
}
