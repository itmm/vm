#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(add_tests, simple) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(20), vm::op_add_int
	};
	signed char expected[] { RAW_INT(30) };
	EXPECT_STACK(code, expected);
}

TEST(add_tests, negative) {
	signed char code[] {
		PUSH_SMALL_INT(-10), PUSH_SMALL_INT(0), vm::op_add_int
	};
	signed char expected[] { RAW_INT(-10) };
	EXPECT_STACK(code, expected);
}

TEST(add_tests, big_ok_a) {
	signed char code[] {
		PUSH_INT(0x7ffffffd), PUSH_SMALL_INT(2), vm::op_add_int
	};
	signed char expected[] { RAW_INT(0x7fffffff) };
	EXPECT_STACK(code, expected);
}

TEST(add_tests, big_overflow_a) {
	signed char code[] {
		PUSH_INT(0x7ffffffd), PUSH_SMALL_INT(3), vm::op_add_int
	};
	EXPECT_ERROR(code, vm::Error::err_add_int_overflow);
}

TEST(add_tests, big_ok_b) {
	signed char code[] {
		PUSH_SMALL_INT(2), PUSH_INT(0x7ffffffd), vm::op_add_int
	};
	signed char expected[] { RAW_INT(0x7fffffff) };
	EXPECT_STACK(code, expected);
}

TEST(add_tests, big_overflow_b) {
	signed char code[] {
		PUSH_SMALL_INT(3), PUSH_INT(0x7ffffffd), vm::op_add_int
	};
	EXPECT_ERROR(code, vm::Error::err_add_int_overflow);
}