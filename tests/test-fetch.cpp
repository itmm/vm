#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(fetch_tests, fetch_ch) {
	signed char code[] {
		PUSH_CH(10), PUSH_CH(20), PUSH_SMALL_INT(1), vm::op_fetch_ch
	};
	signed char expected[] { 10, 20, 10 };
	EXPECT_LIMITED_STACK(code, 6, expected);
}

TEST(fetch_tests, ch_out_of_range) {
	signed char code[] { PUSH_CH(10), PUSH_SMALL_INT(1), vm::op_fetch_ch };
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}

TEST(fetch_tests, fetch_int) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(20), PUSH_SMALL_INT(4),
		vm::op_fetch_int
	};
	signed char expected[] { RAW_INT(10), RAW_INT(20), RAW_INT(10) };
	EXPECT_LIMITED_STACK(code, 12, expected);
}

TEST(fetch_tests, int_out_of_range) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(4), vm::op_fetch_int
	};
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}