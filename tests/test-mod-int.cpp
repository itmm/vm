#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(mod_int_tests, simple) {
	signed char code[] {
		PUSH_SMALL_INT(23), PUSH_SMALL_INT(5), vm::op_mod_int
	};
	signed char expected[] { RAW_INT(3) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, exact) {
	signed char code[] {
		PUSH_SMALL_INT(20), PUSH_SMALL_INT(4), vm::op_mod_int
	};
	signed char expected[] { RAW_INT(0) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, negative) {
	signed char code[] {
		PUSH_SMALL_INT(-23), PUSH_SMALL_INT(5), vm::op_mod_int
	};
	#if CONFIG_OBERON_MATH
		signed char expected[] { RAW_INT(2) };
	#else
		signed char expected[] { RAW_INT(-3) };
	#endif
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, negative_exact) {
	signed char code[] {
		PUSH_SMALL_INT(-20), PUSH_SMALL_INT(5), vm::op_mod_int
	};
	signed char expected[] { RAW_INT(0) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, big_positive) {
	signed char code[] {
		PUSH_INT(0x7fffffff), PUSH_SMALL_INT(2), vm::op_mod_int
	};
	signed char expected[] { RAW_INT(1) };
	EXPECT_STACK(code, expected);
	}

TEST(mod_int_tests, big_negative) {
	signed char code[] {
		PUSH_INT(0x80000000), PUSH_SMALL_INT(2), vm::op_mod_int
	};
	signed char expected[] { RAW_INT(0) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, overflow) {
	signed char code[] {
		PUSH_SMALL_INT(0), PUSH_INT(0x80000000), vm::op_mod_int
	};
	signed char expected[] { RAW_INT(0) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, div_by_0) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(0), vm::op_mod_int
	};
	EXPECT_ERROR(code, vm::Error::err_mod_int_divide_by_0);
}

TEST(mod_int_tests, no_ram) {
	signed char code[] { PUSH_SMALL_INT(8), PUSH_SMALL_INT(2), vm::op_mod_int };
	EXPECT_STACK_OVERFLOW(code, 7);
}

TEST(mod_int_tests, underflow) {
	signed char code[] {
		PUSH_SMALL_INT(5), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), vm::op_mod_int
	};
	EXPECT_ERROR(code, vm::Error::err_leave_stack_segment);
}
