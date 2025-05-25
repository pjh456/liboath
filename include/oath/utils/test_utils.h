#pragma once

#include <chrono>
#include <type_traits>
#include <iostream>
#include <typeinfo>

#define FUNC_TIME_COUNTER(func) \
    oath::utils::function_time_counter(#func, func)

namespace oath::utils
{
	template<typename F>
		requires std::is_invocable_v<F>
	void function_time_counter(const char* func_name, F&& func)
	{
		std::cout << func_name << " started." << std::endl;

		auto start_time = std::chrono::high_resolution_clock::now();

		std::invoke(func);

		auto end_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> duration = end_time - start_time;

		std::cout << func_name << " passed! Spend " << duration.count() << " ms." << std::endl;
	}
}