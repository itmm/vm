#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	TEST(less_tests, equals_ch) {
		signed char code[] { PUSH_BYTE(5), PUSH_BYTE(5), op_less };
		signed char expected[] { RAW_BYTE(false_lit) };
		EXPECT_LIMITED_STACK(code, 2 * Byte::typed_size, expected);
	}

	TEST(less_tests, less_ch) {
		signed char code[] { PUSH_BYTE(4), PUSH_BYTE(5), op_less };
		signed char expected[] { RAW_BYTE(true_lit) };
		EXPECT_LIMITED_STACK(code, 2 * Byte::typed_size, expected);
	}

	TEST(less_tests, underflow_ch) {
		signed char code[] { PUSH_BYTE(4), op_less };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif

#if CONFIG_WITH_INT
	TEST(less_tests, equals_int) {
		signed char code[] { PUSH_SMALL_INT(5), PUSH_SMALL_INT(5), op_less };
		signed char expected[] { RAW_BYTE(false_lit) };
		EXPECT_LIMITED_STACK(code, 2 * Int::typed_size, expected);
	}

	TEST(less_tests, less_int) {
		signed char code[] { PUSH_SMALL_INT(4), PUSH_SMALL_INT(5), op_less };
		signed char expected[] { RAW_BYTE(true_lit) };
		EXPECT_LIMITED_STACK(code, 2 * Int::typed_size, expected);
	}

	TEST(less_tests, underflow_int) {
		signed char code[] { PUSH_SMALL_INT(1), op_less };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif
