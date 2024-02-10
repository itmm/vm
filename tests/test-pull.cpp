#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(pull_tests, pull_ch) {
	signed char code[] { PUSH_CH(10), PUSH_CH(11), vm::op_pull_ch };
	signed char expected[] { 10 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(pull_tests, underflow_ch) {
	signed char code[] { vm::op_pull_ch };
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}

TEST(pull_tests, pull_int) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(11), vm::op_pull_int
	};
	signed char expected[] { RAW_INT(10) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(pull_tests, underflow_int) {
	signed char code[] { PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), vm::op_pull_int };
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}
