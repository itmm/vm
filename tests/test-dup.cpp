#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(dup_tests, dup_ch) {
	signed char code[] { PUSH_CH(10), vm::op_dup_ch };
	signed char expected[] { 10, 10 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(dup_tests, empty_ch) {
	signed char code[] { vm::op_dup_ch };
	EXPECT_ERROR(code, vm::Error::err_leave_stack_segment);
}

TEST(dup_tests, ch_no_ram) {
	signed char code[] { PUSH_CH(22), vm::op_dup_ch };
	EXPECT_STACK_OVERFLOW(code, 1);
}

TEST(dup_tests, dup_int) {
	signed char code[] { PUSH_SMALL_INT(10), vm::op_dup_int };
	signed char expected[] { RAW_INT(10), RAW_INT(10) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(dup_tests, empty_int) {
	signed char code[] { vm::op_dup_int };
	EXPECT_ERROR(code, vm::Error::err_leave_stack_segment);
}

TEST(dup_tests, int_no_ram) {
	signed char code[] { PUSH_SMALL_INT(22), vm::op_dup_int };
	EXPECT_STACK_OVERFLOW(code, 7);
}

