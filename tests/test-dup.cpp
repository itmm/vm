#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(dup_tests, dup_ch) {
	signed char code[] { PUSH_CH(10), op_dup };
	signed char expected[] { RAW_CH(10), RAW_CH(10) };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(dup_tests, empty_ch) {
	signed char code[] { op_dup };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(dup_tests, ch_no_ram) {
	signed char code[] { PUSH_CH(22), op_dup };
	EXPECT_STACK_OVERFLOW(code, 2 * ch_size - 1);
}

TEST(dup_tests, dup_int) {
	signed char code[] { PUSH_SMALL_INT(10), op_dup };
	signed char expected[] { RAW_INT(10), RAW_INT(10) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(dup_tests, empty_int) {
	signed char code[] { op_dup };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(dup_tests, int_no_ram) {
	signed char code[] { PUSH_SMALL_INT(22), op_dup };
	EXPECT_STACK_OVERFLOW(code, 2 * int_size - 1);
}

