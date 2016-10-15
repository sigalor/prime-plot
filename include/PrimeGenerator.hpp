#pragma once



#include <iostream>
#include <vector>
//#include <thread>



namespace PrimeGenerator
{
	long										integerSqrt(long num);
	std::vector<long>							findPrimes(long start, std::size_t num, long* lastCurrLimit, long end=-1);
}
