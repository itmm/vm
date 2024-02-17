#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(div_int_tests, simple) {
	signed char code[] { PUSH_SMALL_INT(20), PUSH_SMALL_INT(4), op_div_int };
	signed char expected[] { RAW_INT(5) };
	EXPECT_STACK(code, expected);
}

TEST(div_int_tests, rounded) {
	signed char code[] { PUSH_SMALL_INT(23), PUSH_SMALL_INT(4), op_div_int };
	signed char expected[] { RAW_INT(5) };
	EXPECT_STACK(code, expected);
}

TEST(div_int_tests, negative) {
	signed char code[] { PUSH_SMALL_INT(-20), PUSH_SMALL_INT(5), op_div_int };
	signed char expected[] { RAW_INT(-4) };
	EXPECT_STACK(code, expected);
}

TEST(div_int_tests, negative_rounded) {
	signed char code[] { PUSH_SMALL_INT(-23), PUSH_SMALL_INT(5), op_div_int };
	#if CONFIG_OBERON_MATH
		signed char expected[] { RAW_INT(-5) };
	#else
		signed char expected[] { RAW_INT(-4) };
	#endif
	EXPECT_STACK(code, expected);
}

TEST(div_int_tests, big_ok_a) {
	signed char code[] { PUSH_INT(0x7fffffff), PUSH_SMALL_INT(1), op_div_int };
	signed char expected[] { RAW_INT(0x7fffffff) };
	EXPECT_STACK(code, expected);
	}

TEST(div_int_tests, big_neg_a) {
	signed char code[] { PUSH_INT(0x80000000), PUSH_SMALL_INT(1), op_div_int };
	signed char expected[] { RAW_INT(0x80000000) };
	EXPECT_STACK(code, expected);
}

TEST(div_int_tests, overflow) {
	signed char code[] { PUSH_INT(0x80000000), PUSH_SMALL_INT(-1), op_div_int };
	EXPECT_ERROR(code, Error::err_div_int_overflow);
}

TEST(div_int_tests, div_by_0) {
	signed char code[] { PUSH_SMALL_INT(10), PUSH_SMALL_INT(0), op_div_int };
	EXPECT_ERROR(code, Error::err_div_int_divide_by_0);
}

TEST(div_int_tests, no_ram) {
	signed char code[] { PUSH_SMALL_INT(8), PUSH_SMALL_INT(2), op_div_int };
	EXPECT_STACK_OVERFLOW(code, 2 * int_size - 1);
}

TEST(div_int_tests, underflow) {
	signed char code[] {
		PUSH_SMALL_INT(5), PUSH_CH(0), op_div_int
	};
	EXPECT_ERROR(code, Error::err_no_integer);
}
