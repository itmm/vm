#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_CHAR && CONFIG_WITH_INT
	TEST(to_ch_tests, zero) {
		signed char code[] { PUSH_INT(0), op_to_ch };
		signed char expected[] { RAW_CH(0) };
		EXPECT_STACK(code, expected);
	}

	TEST(to_ch_tests, simple) {
		signed char code[] { PUSH_INT(10), op_to_ch };
		signed char expected[] { RAW_CH(10) };
		EXPECT_STACK(code, expected);
	}

	TEST(to_ch_tests, max_positive) {
		signed char code[] { PUSH_INT(127), op_to_ch };
		signed char expected[] { RAW_CH(127) };
		EXPECT_STACK(code, expected);
	}

	TEST(to_ch_tests, negative) {
		signed char code[] { PUSH_INT(-10), op_to_ch };
		signed char expected[] { RAW_CH(-10) };
		EXPECT_STACK(code, expected);
	}

	TEST(to_ch_tests, max_negative) {
		signed char code[] { PUSH_INT(-128), op_to_ch };
		signed char expected[] { RAW_CH(-128) };
		EXPECT_STACK(code, expected);
	}

	TEST(to_ch_tests, no_ram) {
		signed char code[] { op_to_ch };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}

	TEST(to_ch_tests, overflow) {
		signed char code[] { PUSH_INT(128), op_to_ch };
		EXPECT_ERROR(code, Err::to_ch_overflow);
	}

	TEST(to_ch_tests, underflow) {
		signed char code[] { PUSH_INT(-129), op_to_ch };
		EXPECT_ERROR(code, Err::to_ch_underflow);
	}
#endif
