#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(add_int_tests, simple) {
	signed char code[] { PUSH_SMALL_INT(10), PUSH_SMALL_INT(20), op_add };
	signed char expected[] { RAW_INT(30) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(add_int_tests, negative) {
	signed char code[] { PUSH_SMALL_INT(-10), PUSH_SMALL_INT(0), op_add };
	signed char expected[] { RAW_INT(-10) };
	EXPECT_STACK(code, expected);
}

TEST(add_int_tests, big_ok_a) {
	signed char code[] { PUSH_INT(0x7ffffffd), PUSH_SMALL_INT(2), op_add };
	signed char expected[] { RAW_INT(0x7fffffff) };
	EXPECT_STACK(code, expected);
}

TEST(add_int_tests, big_overflow_a) {
	signed char code[] { PUSH_INT(0x7ffffffd), PUSH_SMALL_INT(3), op_add };
	EXPECT_ERROR(code, Error::add_overflow);
}

TEST(add_int_tests, big_ok_b) {
	signed char code[] { PUSH_SMALL_INT(2), PUSH_INT(0x7ffffffd), op_add };
	signed char expected[] { RAW_INT(0x7fffffff) };
	EXPECT_STACK(code, expected);
}

TEST(add_int_tests, big_overflow_b) {
	signed char code[] { PUSH_SMALL_INT(3), PUSH_INT(0x7ffffffd), op_add };
	EXPECT_ERROR(code, Error::add_overflow);
}

TEST(add_int_tests, two_bigs) {
	signed char code[] { PUSH_INT(0x7fffffff), PUSH_INT(0x80000000), op_add };
	signed char expected[] { RAW_INT(-1) };
	EXPECT_STACK(code, expected);
}

TEST(add_int_tests, no_ram) {
	signed char code[] { PUSH_SMALL_INT(5), PUSH_SMALL_INT(6), op_add };
	EXPECT_STACK_OVERFLOW(code, 7);
}

TEST(add_int_tests, underflow) {
	signed char code[] { PUSH_SMALL_INT(5), op_add };
	EXPECT_ERROR(code, Error::leave_stack_segment);
}
