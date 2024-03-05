#include "gtest/gtest.h"

#include "ops/div.h"
#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_INT
	TEST(div_tests, simple) {
		signed char code[] { PUSH_SMALL_INT(20), PUSH_SMALL_INT(4), op_div };
		signed char expected[] { RAW_INT(5) };
		EXPECT_STACK(code, expected);
	}

	TEST(div_tests, rounded) {
		EXPECT_EQ(ops::Div { } (Int { 23 }, Int { 4 } ), Value { Int { 5 } });
	}

	TEST(div_tests, negative) {
		EXPECT_EQ(ops::Div { } (Int { -20 }, Int { 5 }), Value { Int { -4 } });
	}

	TEST(div_tests, negative_rounded) {
		#if CONFIG_OBERON_MATH
			Int expected { -5 };
		#else
			Int expected { -4 };
		#endif
		EXPECT_EQ(ops::Div { } (Int { -23 }, Int { 5 }), Value { expected });
	}

	TEST(div_tests, big_ok_a) {
		EXPECT_EQ(
			ops::Div { } (Int { 0x7fffffff }, Int { 1 }),
			Value { Int { 0x7fffffff }}
		);
	}

	TEST(div_tests, big_neg_a) {
		Int big_neg { static_cast<int>(0x80000000) };
		EXPECT_EQ(ops::Div { } (big_neg, Int { 1 }), Value { big_neg });
	}

	TEST(div_tests, overflow) {
		signed char code[] { PUSH_INT(0x80000000), PUSH_SMALL_INT(-1), op_div };
		EXPECT_ERROR(code, Err::div_overflow);
	}

	TEST(div_tests, div_by_0) {
		signed char code[] { PUSH_SMALL_INT(10), PUSH_SMALL_INT(0), op_div };
		EXPECT_ERROR(code, Err::div_divide_by_0);
	}

	TEST(div_tests, no_ram) {
		signed char code[] { PUSH_SMALL_INT(8), PUSH_SMALL_INT(2), op_div };
		EXPECT_STACK_OVERFLOW(code, 2 * Int::typed_size - 1);
	}

	TEST(div_tests, underflow) {
		signed char code[] { PUSH_SMALL_INT(5), op_div };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif