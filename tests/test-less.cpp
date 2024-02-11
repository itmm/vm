#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(less_tests, equals_ch) {
	signed char code[] { PUSH_CH(5), PUSH_CH(5), vm::op_less_ch };
	signed char expected[] { 0 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(less_tests, less_ch) {
	signed char code[] { PUSH_CH(4), PUSH_CH(5), vm::op_less_ch };
	signed char expected[] { -1 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(less_tests, underflow_ch) {
	signed char code[] { PUSH_CH(4), vm::op_less_ch };
	EXPECT_ERROR(code, vm::Error::err_leave_stack_segment);
}

TEST(less_tests, equals_int) {
	signed char code[] {
		PUSH_SMALL_INT(5), PUSH_SMALL_INT(5), vm::op_less_int
	};
	signed char expected[] { 0 };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(less_tests, less_int) {
	signed char code[] {
		PUSH_SMALL_INT(4), PUSH_SMALL_INT(5), vm::op_less_int
	};
	signed char expected[] { -1 };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(less_tests, underflow_int) {
	signed char code[] {
		PUSH_SMALL_INT(1), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), vm::op_less_int
	};
	EXPECT_ERROR(code, vm::Error::err_leave_stack_segment);
}
