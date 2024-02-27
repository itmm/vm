#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_CHAR
	TEST(pull_tests, pull_ch) {
		signed char code[] { PUSH_CH(10), PUSH_CH(11), op_pull };
		signed char expected[] { RAW_CH(10) };
		EXPECT_LIMITED_STACK(code, 2 * Char::typed_size, expected);
	}
#endif

TEST(pull_tests, underflow) {
	signed char code[] { op_pull };
	EXPECT_ERROR(code, Err::leave_stack_segment);
}

#if CONFIG_WITH_INT
	TEST(pull_tests, pull_int) {
		signed char code[] { PUSH_SMALL_INT(10), PUSH_SMALL_INT(11), op_pull };
		signed char expected[] { RAW_INT(10) };
		EXPECT_LIMITED_STACK(code, 2 * Int::typed_size, expected);
	}
#endif
