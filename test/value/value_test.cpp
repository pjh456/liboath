#include "value/value.h"
#include "utils/test_utils.h"
#include "utils/concept_utils.h"

#include <stdio.h>
#include <cassert>
#include <string>

namespace oath::value::test
{
	template<typename T>
		requires(Addable<T>, Comparable<T>)
	void test_value_map_T(const T& add1, const T& add2)
	{
		Value<T> a(add1);
		auto add_func = [add2](const T& val) { return val + add2; };
		auto add_move_func = [add2](T&& val) { return std::move(val) + add2; };

		auto b = a.map(add_func);
		assert(b.get() == a.get() + add2);

		auto c = std::move(a).map(add_move_func);
		assert(c.get() == add1 + add2);

		auto d = a.map(add_func).map(add_func);
		assert(d.get() == a.get() + add2 + add2);
	}

	void test_value_map()
	{
		test_value_map_T(1, 1);
		test_value_map_T(1.0, 1.0);
		test_value_map_T(std::string("Hello "), std::string("world!"));
	}

	void test_value()
	{
		FUNC_TIME_COUNTER(test_value_map);
		getchar();
	}
}