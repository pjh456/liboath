#include "ownership/owned.h"
#include "utils/test_utils.h"

#include <cassert>
#include <stdio.h>
#include <string>

namespace oath::test
{
	void test_owned_assign()
	{
		throw std::exception("test!!!");
	}

	void test_owned()
	{
		FUNC_TIME_COUNTER(test_owned_assign);
		getchar();
	}
}

int main()
{
	oath::test::test_owned();
}