#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(swap_tests, simple) {
	signed char code[] {
		PUSH_SMALL_INT(1), PUSH_SMALL_INT(2), vm::op_swap_int
	};
	signed char expected[] { RAW_INT(1), RAW_INT(2) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(swap_tests, empty_ch) {
	signed char code[] {
		PUSH_SMALL_INT(1), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), vm::op_swap_int
	};
	EXPECT_ERROR(code, vm::Error::err_leave_stack_segment);
}
