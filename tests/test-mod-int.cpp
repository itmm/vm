#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(mod_int_tests, simple) {
	signed char code[] { PUSH_SMALL_INT(23), PUSH_SMALL_INT(5), op_mod };
	signed char expected[] { RAW_INT(3) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, exact) {
	signed char code[] { PUSH_SMALL_INT(20), PUSH_SMALL_INT(4), op_mod };
	signed char expected[] { RAW_INT(0) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, negative) {
	signed char code[] { PUSH_SMALL_INT(-23), PUSH_SMALL_INT(5), op_mod };
	#if CONFIG_OBERON_MATH
		signed char expected[] { RAW_INT(2) };
	#else
		signed char expected[] { RAW_INT(-3) };
	#endif
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, negative_exact) {
	signed char code[] { PUSH_SMALL_INT(-20), PUSH_SMALL_INT(5), op_mod };
	signed char expected[] { RAW_INT(0) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, big_positive) {
	signed char code[] { PUSH_INT(0x7fffffff), PUSH_SMALL_INT(2), op_mod };
	signed char expected[] { RAW_INT(1) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, big_negative) {
	signed char code[] { PUSH_INT(0x80000000), PUSH_SMALL_INT(2), op_mod };
	signed char expected[] { RAW_INT(0) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, overflow) {
	signed char code[] { PUSH_SMALL_INT(0), PUSH_INT(0x80000000), op_mod };
	signed char expected[] { RAW_INT(0) };
	EXPECT_STACK(code, expected);
}

TEST(mod_int_tests, div_by_0) {
	signed char code[] { PUSH_SMALL_INT(10), PUSH_SMALL_INT(0), op_mod };
	EXPECT_ERROR(code, Err::mod_divide_by_0);
}

TEST(mod_int_tests, underflow) {
	signed char code[] { PUSH_SMALL_INT(5), op_mod };
	EXPECT_ERROR(code, Err::leave_stack_segment);
}
