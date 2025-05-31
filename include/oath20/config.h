#pragma once

#define OATH_RUNTIME_CHECK

#define OATH_DEBUG
//#define OATH_RELEASE

#ifdef OATH_RUNTIME_CHECK

#include "utils/runtime_check_utils.h"
	#define OATH_CHECK(expr) \
		do \
		{ \
			if(!(expr)) [[unlikely]] oath::detail::runtime_check_failed(#expr, __FILE__, __LINE__); \
		} \
		while(0)

	#define OATH_NOEXCEPT
#else
	#define OATH_CHECK(expr) ((void)0)

	#define OATH_NOEXCEPT noexcept
#endif