#pragma once

#include <type_traits>
#include <concepts>

namespace oath
{
	template<typename T>
	concept Addable = requires(T a, T b) {
		{ a + b } -> std::convertible_to<T>;
	};

	template<typename T>
	concept Comparable = requires(const T & a, const T & b)
	{
		{ a == b } -> std::convertible_to<bool>;
	};
}