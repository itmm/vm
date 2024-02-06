#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(push_pull_tests, empty) {
	signed char code[] { vm::op_nop };
	signed char expected[] {};
	EXPECT_STACK(code, expected);
}

TEST(push_pull_tests, push_3) {
	signed char code[] { PUSH_CH(10), PUSH_CH(20), PUSH_CH(30) };
	signed char expected[] { 30, 20, 10 };
	EXPECT_STACK(code, expected);
}