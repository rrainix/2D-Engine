#pragma once
#include <random>
#include <type_traits>
#include <cstdint>

namespace rnd {
	static thread_local std::mt19937_64 gen(std::random_device{}());

	inline void seed(long s) {
		gen.seed(s);
	}

	template<typename T>
	inline T next(T min, T max) {
		// Wähle zur Compile-Zeit die passende Distribution
		using dist_t = std::conditional_t<
			std::is_integral_v<T>,
			std::uniform_int_distribution<T>,
			std::uniform_real_distribution<T>
		>;

		// Einmalige Erzeugung der Distribution pro Thread
		static thread_local dist_t dist;

		// Nur die Parameter (min, max) neu setzen – minimaler Overhead
		return dist(gen, typename dist_t::param_type(min, max));
	}

}