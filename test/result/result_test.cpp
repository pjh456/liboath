#include "result/result.h"
#include "utils/test_utils.h"

#include <stdio.h>
#include <cassert>
#include <string>

namespace oath::test
{
	void test_result_get()
	{
		auto func = [](bool val) {
			return val ?
				oath::Result<bool, std::string>::ok(true) :
				oath::Result<bool, std::string>::err(std::string("false"));
			};

		auto result1 = func(true);
		assert(std::move(result1).unwrap() == true);

		auto result2 = func(false);
		assert(std::move(result2).unwrap_err() == "false");
	}

	void test_result_map()
	{
		auto func = [](bool val) {
			return val ?
				oath::Result<bool, std::string>::ok(true) :
				oath::Result<bool, std::string>::err(std::string("false"));
			};

		auto map_func1 = [](const bool& val) { return val ? 1 : 0; };

		auto result1 = func(true).map(map_func1)
			.map_err([](const std::string& val) { return std::move(val.empty()); })
			.and_then([](int val) { return Result<int, bool>::err(true); })
			.unwrap_err();
		assert(result1 == true);

		auto result2 = func(false)
			.map(map_func1);
		assert(result2.is_err());
	}

	void test_result_match()
	{
		auto func = [](const bool& val) {
			return val ?
				oath::Result<bool, std::string>::ok(true) :
				oath::Result<bool, std::string>::err(std::string("false"));
			};

		auto match_func1 = [](bool&& val) { return true; };
		auto match_func2 = [](std::string&& val) { return false; };

		auto result1 = func(true).match(match_func1, match_func2);
		assert(result1 == true);

		auto result2 = func(false).match(match_func1, match_func2);
		assert(result2 == false);
	}

	void test_result()
	{
		FUNC_TIME_COUNTER(test_result_get);
		FUNC_TIME_COUNTER(test_result_map);
		getchar();
	}
}