#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_INT
	TEST(mult_tests, simple) {
		signed char code[] { PUSH_SMALL_INT(20), PUSH_SMALL_INT(30), op_mult };
		signed char expected[] { RAW_INT(600) };
		EXPECT_STACK(code, expected);
	}

	TEST(mult_tests, negative) {
		signed char code[] { PUSH_SMALL_INT(-20), PUSH_SMALL_INT(30), op_mult };
		signed char expected[] { RAW_INT(-600) };
		EXPECT_STACK(code, expected);
	}

	TEST(mult_tests, big_ok_a) {
		signed char code[] { PUSH_INT(0x7fffffff), PUSH_SMALL_INT(1), op_mult };
		signed char expected[] { RAW_INT(0x7fffffff) };
		EXPECT_STACK(code, expected);
	}

	TEST(mult_tests, overflow) {
		signed char code[] { PUSH_INT(0x80000000), PUSH_SMALL_INT(-1), op_mult };
		EXPECT_ERROR(code, Err::mult_overflow);
	}

	TEST(mult_tests, big_overflow_a) {
		signed char code[] { PUSH_INT(0x7fffffff), PUSH_SMALL_INT(2), op_mult };
		EXPECT_ERROR(code, Err::mult_overflow);
	}

	TEST(mult_tests, big_ok_b) {
		signed char code[] { PUSH_SMALL_INT(1), PUSH_INT(0x7fffffff), op_mult };
		signed char expected[] { RAW_INT(0x7fffffff) };
		EXPECT_STACK(code, expected);
	}

	TEST(mult_tests, big_overflow_b) {
		signed char code[] { PUSH_SMALL_INT(2), PUSH_INT(0x7fffffff), op_mult };
		EXPECT_ERROR(code, Err::mult_overflow);
	}

	TEST(mult_tests, two_bigs_overflow) {
		signed char code[] { PUSH_INT(0x8000), PUSH_INT(0x10000), op_mult };
		EXPECT_ERROR(code, Err::mult_overflow);
	}

	TEST(mult_tests, max_neg) {
		signed char code[] { PUSH_INT(0x8000), PUSH_INT(-0x10000), op_mult };
		signed char expected[] { RAW_INT(0x80000000) };
		EXPECT_STACK(code, expected);
	}

	TEST(mult_tests, underflow) {
		signed char code[] { PUSH_SMALL_INT(5), op_mult };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif