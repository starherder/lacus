#pragma once

#include <random>
#include <cstdlib>

namespace utility
{
	class RandomHelper
	{
	public:
		template<typename T>
		static inline T randomReal(T min, T max)
		{
			std::uniform_real_distribution<T> dist(min, max);
			auto& mt = getEngine();
			return dist(mt);
		}

		template<typename T>
		static inline T randomInt(T min, T max)
		{
			std::uniform_int_distribution<T> dist(min, max);
			auto& mt = getEngine();
			return dist(mt);
		}
	private:
		static std::mt19937& getEngine()
		{
			static std::random_device seed_gen;
			static std::mt19937 engine(seed_gen());
			return engine;
		}
	};

	template<typename T>
	inline T random(T min, T max)
	{
		return RandomHelper::randomInt<T>(min, max);
	}

	template<>
	inline float random(float min, float max)
	{
		return RandomHelper::randomReal(min, max);
	}

	template<>
	inline long double random(long double min, long double max)
	{
		return RandomHelper::randomReal(min, max);
	}

	template<>
	inline double random(double min, double max)
	{
		return RandomHelper::randomReal(min, max);
	}

	// [-1, 1)
	inline float rand_minus1_1()
	{
		return RandomHelper::randomReal(-1.0f, 1.0f);
	}

	// [0, 1)
	inline float rand_0_1()
	{
		return RandomHelper::randomReal(0.0f, 1.0f);
	}

}
