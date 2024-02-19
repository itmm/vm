#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(equals_tests, equals_ch) {
	signed char code[] { PUSH_CH(5), PUSH_CH(5), op_equals };
	signed char expected[] { RAW_CH(true_lit) };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(equals_tests, not_equals_ch) {
	signed char code[] { PUSH_CH(4), PUSH_CH(5), op_equals };
	signed char expected[] { RAW_CH(false_lit) };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(equals_tests, underflow_ch) {
	signed char code[] { PUSH_CH(4), op_equals };
	EXPECT_ERROR(code, Err::leave_stack_segment);
}

TEST(equals_tests, equals_int) {
	signed char code[] { PUSH_SMALL_INT(5), PUSH_SMALL_INT(5), op_equals };
	signed char expected[] { RAW_CH(true_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(equals_tests, not_equals_int) {
	signed char code[] { PUSH_SMALL_INT(4), PUSH_SMALL_INT(5), op_equals };
	signed char expected[] { RAW_CH(false_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(equals_tests, empty_int) {
	signed char code[] { PUSH_SMALL_INT(1), op_equals };
	EXPECT_ERROR(code, Err::leave_stack_segment);
}
