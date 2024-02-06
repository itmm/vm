#include "gtest/gtest.h"

#include "vm-tests.h"
#include "vm.h"

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
