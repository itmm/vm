#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(dup_tests, dup_ch) {
	signed char code[] { PUSH_CH(10), vm::op_dup_ch };
	signed char expected[] { 10, 10 };
	EXPECT_STACK(code, expected);
}

TEST(dup_tests, dup_int) {
	signed char code[] { PUSH_SMALL_INT(10), vm::op_dup_int };
	signed char expected[] { RAW_INT(10), RAW_INT(10) };
	EXPECT_STACK(code, expected);
}
