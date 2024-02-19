#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(ch_to_int_tests, zero) {
	signed char code[] { PUSH_SMALL_INT(0) };
	signed char expected[] { RAW_INT(0) };
	EXPECT_STACK(code, expected);
}

TEST(ch_to_int_tests, simple) {
	signed char code[] { PUSH_SMALL_INT(10) };
	signed char expected[] { RAW_INT(10) };
	EXPECT_STACK(code, expected);
}

TEST(ch_to_int_tests, negative) {
	signed char code[] { PUSH_SMALL_INT(-10) };
	signed char expected[] { RAW_INT(-10) };
	EXPECT_STACK(code, expected);
}

TEST(ch_to_int_tests, no_ram) {
	signed char code[] { PUSH_SMALL_INT(5) };
	EXPECT_STACK_OVERFLOW(code, int_size - 1);
}

TEST(ch_to_int_tests, underflow) {
	signed char code[] { op_to_int };
	EXPECT_ERROR(code, Error::leave_stack_segment);
}
