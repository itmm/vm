#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(pull_tests, pull_ch) {
	signed char code[] { PUSH_CH(10), PUSH_CH(11), op_pull_ch };
	signed char expected[] { 10 };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(pull_tests, underflow_ch) {
	signed char code[] { op_pull_ch };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(pull_tests, pull_int) {
	signed char code[] { PUSH_SMALL_INT(10), PUSH_SMALL_INT(11), op_pull_int };
	signed char expected[] { RAW_INT(10) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(pull_tests, underflow_int) {
	signed char code[] { PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), op_pull_int };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}
