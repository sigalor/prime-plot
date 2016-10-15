#pragma once



#include <cstdlib>
#include <iostream>
#include <vector>



namespace CPPUtilities
{
	template<typename T>
	std::vector<std::vector<T>> create2DVector(std::size_t dim1, std::size_t dim2)
	{
		std::vector<std::vector<T>> ret;

		try
		{
			ret.resize(dim1);
			for(auto& y : ret)
			{
				y.resize(dim2);
				for(auto& x : y)
					x = T();
			}
		}
		catch(...)
		{
			std::cerr << "error: not enough memory\n";
			std::exit(1);
		}
	
		return ret;
	}
	
	template<typename T>
	std::vector<std::vector<T>> create2DVector(std::size_t dim)
	{
		return create2DVector<T>(dim, dim);
	}
}
