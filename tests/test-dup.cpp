#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	TEST(dup_tests, dup_ch) {
		signed char code[] { PUSH_BYTE(10), op_dup };
		signed char expected[] { RAW_BYTE(10), RAW_BYTE(10) };
		EXPECT_LIMITED_STACK(code, 2 * Byte::typed_size, expected);
	}
#endif

TEST(dup_tests, empty) {
	signed char code[] { op_dup };
	EXPECT_ERROR(code, Err::leave_stack_segment);
}

#if CONFIG_WITH_BYTE
	TEST(dup_tests, ch_no_ram) {
		signed char code[] { PUSH_BYTE(22), op_dup };
		EXPECT_STACK_OVERFLOW(code, 2 * Byte::typed_size - 1);
	}
#endif

#if CONFIG_WITH_INT
	TEST(dup_tests, dup_int) {
		signed char code[] { PUSH_SMALL_INT(10), op_dup };
		signed char expected[] { RAW_INT(10), RAW_INT(10) };
		EXPECT_LIMITED_STACK(code, 2 * Int::typed_size, expected);
	}

	TEST(dup_tests, int_no_ram) {
		signed char code[] { PUSH_SMALL_INT(22), op_dup };
		EXPECT_STACK_OVERFLOW(code, 2 * Int::typed_size - 1);
	}
#endif
