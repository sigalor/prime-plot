#include "PrimeGenerator.hpp"



long PrimeGenerator::integerSqrt(long num)																		//this is a nice method to quickly get the integer square root of 'num' if it has one, from http://forum.nasm.us/index.php?topic=1694.0
{																												//note that it only uses addition and subtraction
	long ret = 0;
	long i = 1;

	while(num > 0)																								//repeatedly subtract odd numbers from 'num'
	{																											//examples:
		num -= i;																								//16   --minus 1-->   15   --minus 3-->   12   --minus 5-->    7   --minus 7-->   0    ----->   subtracted 4 odd numbers, 16's integer square root is 4
		i += 2;																									//32   --minus 1-->   31   --minus 3-->   28   --minus 5-->   23   --minus 7-->   16   --minus 9-->   7   --minus 11-->   -4   ----->    number became negative, so it does not have an integer square root
		ret++;
	}

	if(num == 0)																								//if 'num' became zero, the number of subracted odd numbers is its integer square root
		return ret;																								//if 'num' did not become zero (i.e. it became negative), it does not have an integer square root
	return 0;
}

std::vector<long> PrimeGenerator::findPrimes(long start, std::size_t num, long* lastCurrLimit, long end)		//when lastCurrLimit!=nullptr, then start has to be the last prime of the previous call of this function; end will not be included if it's a prime
{
	long i, j;
	long currLimit;
	bool isPrime;
	//std::vector<std::thread> primeTestThreads;
	std::vector<long> ret;

	//TODO: include a (Intel or AT&T) assembly translation for the following algorithm to (possibly) increase performance
	i = start;																									//i loops through all numbers to test
	if(lastCurrLimit == nullptr  ||  *lastCurrLimit == 0)
		for(j=start; (currLimit = integerSqrt(j))==0; j--) { }													//makes currLimit become the next lowest integer square root, e.g. (start --> currLimit): 2 --> 1; 4 --> 2; 28 --> 5; 98 --> 9; 101 --> 10, 121 --> 11
	else
		currLimit = *lastCurrLimit + (integerSqrt(i = start+1)!=0 ? 1 : 0);
	
	while(ret.size() < num)																						//when the function 'find_primes' the array 'buffer' will keep exactly 'size' primes, starting from (and possibly including) 'start'. You cannot define an ending prime in this function
	{
		if(i == (currLimit+1)*(currLimit+1))																	//if the current 'i' reached the square of incremented 'currLimit' (that is, the next integer square is reached)
		{
			currLimit++;																						//increment currLimit
			i++;																								//increment i, because it is clear that 'i' already is the square of a number, so it cannot be a prime
			continue;
		}

		/*isPrime = true;
		checkPrimeStepWidth = 1 + ((currLimit-2+1 - 1) / numThreads);											//fast ceiling of integer division, from http://stackoverflow.com/a/2745086
		for(j=0; j<numThreads; j++)
		{
			checkPrimeStart = 2+j*checkPrimeStepWidth;
			checkPrimeEnd = 2+(j+1)*checkPrimeStepWidth-1;
			if(checkPrimeEnd > currLimit)
				checkPrimeEnd = currLimit;
			primeTestThreads.push_back(std::thread(checkPrime, i, checkPrimeStart, checkPrimeEnd));
		}
		for(j=0; j<numThreads; j++)
			primeTestThreads[j].join();
		primeTestThreads.clear();*/
		
		isPrime = true;
		for(j=2; j<=currLimit; j++)																				//just a standard loop for checking if 'i' is prime. 'j' will be in range [2; floor(sqrt(i))], because of 'currLimit's composition
		{
			if(i%j == 0)
			{
				isPrime = false;
				break;
			}
		}
	
		if(isPrime)																								//only if the current 'i' is prime, ...
		{
			if(end != -1  &&  i >= end)
				break;
			ret.push_back(i);																					//fill the buffer with it
		}
	
		i++;
	}

	if(lastCurrLimit != nullptr)
		*lastCurrLimit = currLimit;
	return ret;
}
