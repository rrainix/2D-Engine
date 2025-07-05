#pragma once
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

namespace math {

	template<typename T>
	struct constants;

	template<>
	struct constants<float> {
		static constexpr float pi = 3.14159265358979323846f;
		static constexpr float double_pi = 6.283185307179586f;
		static constexpr float rad2Deg = 57.29577951308232f;
		static constexpr float deg2Rad = 0.017453292519943295f;
	};

	template<>
	struct constants<double> {
		static constexpr double pi = 3.14159265358979323846;
		static constexpr double double_pi = 6.283185307179586;
		static constexpr double rad2Deg = 57.29577951308232;
		static constexpr double deg2Rad = 0.017453292519943295;
	};

	inline float radians(float degrees) {
		return degrees * constants<float>().deg2Rad; // PI / 180
	}
	inline float degrees(float radians) {
		return radians * constants<float>().rad2Deg; // 180 / PI
	}

	template<typename T>
	inline T lerp(T a, T b, T t) {
		return a + t * (b - a);
	}
	inline float distance(float a, float b) {
		return std::fabs(b - a);
	}

	template<typename T>
	static T ceil(T a) {
		return  std::ceil<T>(a);
	}

	template<typename T>
	static T floor(T a) {
		return std::floor<T>(a);
	}

	template<typename T>
	static T round(T a) {
		return  std::round<T>(a);
	}

	template<typename T>
	inline T abs(T a) {
		return a < 0 ? a * -1 : a;
	}

	template<typename T>
	T evaluate(const std::string& expr) {
		std::istringstream iss(expr);
		T result = 0, value = 0;
		char op = '+';

		while (iss >> value) {
			if (op == '+') result += value;
			else if (op == '-') result -= value;
			else if (op == '*') result *= value;
			else if (op == '/') result /= value;

			iss >> op;
		}

		return result;
	}

	template<typename T>
	inline T pow(T baseValue, T exponent) {
		return  std::pow<T>(baseValue, exponent);
	}

	template<typename T>
	inline T sqrt(T value) {
		return std::sqrt<T>(value);
	}

	template<typename T>
	inline T clamp(T x, T minVal, T maxVal) {
		return x < minVal ? minVal : (x > maxVal ? maxVal : x);
	}
	template<typename T>
	inline T max(T a, T b) {
		return (a > b) ? a : b;
	}
	template<typename T>
	inline T min(T a, T b) {
		return (a < b) ? a : b;
	}
	template<typename T, typename... Args>
	T max(T a, Args... args) {
		T b = max(args...);
		return (a > b) ? a : b;
	}
	template<typename T, typename... Args>
	T min(T a, Args... args) {
		T b = min(args...);
		return (a < b) ? a : b;
	}
}
