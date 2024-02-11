#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(sub_int_tests, simple) {
	signed char code[] {
		PUSH_SMALL_INT(30), PUSH_SMALL_INT(20), vm::op_sub_int
	};
	signed char expected[] { RAW_INT(10) };
	EXPECT_STACK(code, expected);
}

TEST(sub_int_tests, negative) {
	signed char code[] {
		PUSH_SMALL_INT(-10), PUSH_SMALL_INT(20), vm::op_sub_int
	};
	signed char expected[] { RAW_INT(-30) };
	EXPECT_STACK(code, expected);
}

TEST(sub_int_tests, big_ok_a) {
	signed char code[] {
		PUSH_INT(0x7ffffffd), PUSH_SMALL_INT(-2), vm::op_sub_int
	};
	signed char expected[] { RAW_INT(0x7fffffff) };
	EXPECT_STACK(code, expected);
}

TEST(sub_int_tests, big_overflow_a) {
	signed char code[] {
		PUSH_INT(0x7ffffffd), PUSH_SMALL_INT(-3), vm::op_sub_int
	};
	EXPECT_ERROR(code, vm::Error::err_sub_int_overflow);
}

TEST(sub_int_tests, big_ok_b) {
	signed char code[] {
		PUSH_SMALL_INT(2), PUSH_INT(-0x7ffffffd), vm::op_sub_int
	};
	signed char expected[] { RAW_INT(0x7fffffff) };
	EXPECT_STACK(code, expected);
}

TEST(sub_int_tests, big_overflow_b) {
	signed char code[] {
		PUSH_SMALL_INT(3), PUSH_INT(-0x7ffffffd), vm::op_sub_int
	};
	EXPECT_ERROR(code, vm::Error::err_sub_int_overflow);
}

TEST(sub_int_tests, two_bigs) {
	signed char code[] {
		PUSH_INT(0x7fffffff), PUSH_INT(0x7ffffffd), vm::op_sub_int
	};
	signed char expected[] { RAW_INT(2) };
	EXPECT_STACK(code, expected);
}

TEST(sub_int_tests, no_ram) {
	signed char code[] { PUSH_SMALL_INT(8), PUSH_SMALL_INT(2), vm::op_sub_int };
	EXPECT_STACK_OVERFLOW(code, 7);
}

TEST(sub_int_tests, underflow) {
	signed char code[] {
		PUSH_SMALL_INT(5), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), vm::op_sub_int
	};
	EXPECT_ERROR(code, vm::Error::err_leave_stack_segment);
}
