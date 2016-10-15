#include "UnicodeUtilities.hpp"



void UnicodeUtilities::outputUnicodeAsUtf8(std::ostream& output, uint32_t u)										//explanation from http://stackoverflow.com/a/6240184
{
	if(u <= 0x7F)
		output << (char)u;
	else if(u <= 0x7FF)
		output << (char)(0b11000000 | ((u >> 6) & 0b11111))
			   << (char)(0b10000000 | (u & 0b111111));
	else if(u <= 0xFFFF)
		output << (char)(0b11100000 | ((u >> 12) & 0b1111))
			   << (char)(0b10000000 | ((u >> 6) & 0b111111))
			   << (char)(0b10000000 | (u & 0b111111));
	else if(u <= 0x1FFFFF)
		output << (char)(0b11110000 | ((u >> 18) & 0b111))
			   << (char)(0b10000000 | ((u >> 12) & 0b111111))
			   << (char)(0b10000000 | ((u >> 6) & 0b111111))
			   << (char)(0b10000000 | (u & 0b111111));
	else if(u <= 0x3FFFFFF)
		output << (char)(0b11111000 | ((u >> 24) & 0b11))
			   << (char)(0b10000000 | ((u >> 18) & 0b111111))
			   << (char)(0b10000000 | ((u >> 12) & 0b111111))
			   << (char)(0b10000000 | ((u >> 6) & 0b111111))
			   << (char)(0b10000000 | (u & 0b111111));
	else if(u <= 0x7FFFFFFF)
		output << (char)(0b11111100 | ((u >> 30) & 0b1))
			   << (char)(0b10000000 | ((u >> 24) & 0b111111))
			   << (char)(0b10000000 | ((u >> 18) & 0b111111))
			   << (char)(0b10000000 | ((u >> 12) & 0b111111))
			   << (char)(0b10000000 | ((u >> 6) & 0b111111))
			   << (char)(0b10000000 | (u & 0b111111));
}
