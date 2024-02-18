#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(less_tests, equals_ch) {
	signed char code[] { PUSH_CH(5), PUSH_CH(5), op_less };
	signed char expected[] { RAW_CH(false_lit) };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(less_tests, less_ch) {
	signed char code[] { PUSH_CH(4), PUSH_CH(5), op_less };
	signed char expected[] { RAW_CH(true_lit) };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(less_tests, underflow_ch) {
	signed char code[] { PUSH_CH(4), op_less };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(less_tests, equals_int) {
	signed char code[] { PUSH_SMALL_INT(5), PUSH_SMALL_INT(5), op_less };
	signed char expected[] { RAW_CH(false_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(less_tests, less_int) {
	signed char code[] { PUSH_SMALL_INT(4), PUSH_SMALL_INT(5), op_less };
	signed char expected[] { RAW_CH(true_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(less_tests, underflow_int) {
	signed char code[] { PUSH_SMALL_INT(1), op_less };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}
