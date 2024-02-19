#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(swap_tests, simple) {
	signed char code[] { PUSH_SMALL_INT(1), PUSH_SMALL_INT(2), op_swap };
	signed char expected[] { RAW_INT(1), RAW_INT(2) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(swap_tests, overflow) {
	signed char code[] { PUSH_SMALL_INT(1), op_swap };
	EXPECT_ERROR(code, Err::leave_stack_segment);
}
