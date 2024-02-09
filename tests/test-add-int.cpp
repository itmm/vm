#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(add_int_tests, simple) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(20), vm::op_add_int
	};
	signed char expected[] { RAW_INT(30) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(add_int_tests, negative) {
	signed char code[] {
		PUSH_SMALL_INT(-10), PUSH_SMALL_INT(0), vm::op_add_int
	};
	signed char expected[] { RAW_INT(-10) };
	EXPECT_STACK(code, expected);
}

TEST(add_int_tests, big_ok_a) {
	signed char code[] {
		PUSH_INT(0x7ffffffd), PUSH_SMALL_INT(2), vm::op_add_int
	};
	signed char expected[] { RAW_INT(0x7fffffff) };
	EXPECT_STACK(code, expected);
}

TEST(add_int_tests, big_overflow_a) {
	signed char code[] {
		PUSH_INT(0x7ffffffd), PUSH_SMALL_INT(3), vm::op_add_int
	};
	EXPECT_ERROR(code, vm::Error::err_add_int_overflow);
}

TEST(add_int_tests, big_ok_b) {
	signed char code[] {
		PUSH_SMALL_INT(2), PUSH_INT(0x7ffffffd), vm::op_add_int
	};
	signed char expected[] { RAW_INT(0x7fffffff) };
	EXPECT_STACK(code, expected);
}

TEST(add_int_tests, big_overflow_b) {
	signed char code[] {
		PUSH_SMALL_INT(3), PUSH_INT(0x7ffffffd), vm::op_add_int
	};
	EXPECT_ERROR(code, vm::Error::err_add_int_overflow);
}

TEST(add_int_tests, two_bigs) {
	signed char code[] {
		PUSH_INT(0x7fffffff), PUSH_INT(0x80000000), vm::op_add_int
	};
	signed char expected[] { RAW_INT(-1) };
	EXPECT_STACK(code, expected);
}

TEST(add_int_tests, no_ram) {
	signed char code[] { PUSH_SMALL_INT(5), PUSH_SMALL_INT(6), vm::op_add_int };
	EXPECT_STACK_OVERFLOW(code, 7);
}

TEST(add_int_tests, underflow) {
	signed char code[] {
		PUSH_SMALL_INT(5), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), vm::op_add_int
	};
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}
