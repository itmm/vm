#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_INT
	TEST(swap_tests, simple) {
		signed char code[] { PUSH_SMALL_INT(1), PUSH_SMALL_INT(2), op_swap };
		signed char expected[] { RAW_INT(1), RAW_INT(2) };
		EXPECT_LIMITED_STACK(code, 2 * Int::typed_size, expected);
	}

	TEST(swap_tests, overflow) {
		signed char code[] { PUSH_SMALL_INT(1), op_swap };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif
