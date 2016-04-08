#pragma once
#include <iostream>
#include <random>

namespace Utils {

	template<class T, typename U, typename V>
	inline T RandomNumber(U a, V b)
	{
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 eng(rd()); // seed the generator
		std::uniform_int_distribution<> distr(a, b); // define the range

		return distr(eng);
	}

	template<class T, class U, class V, class W>
	inline T lerp(U a, V b, W t) 
	{ 
		return (1 - t)* a + t*b; 
	};

}

