#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(push_pull_tests, empty) {
	signed char code[] { }, expected[] {};
	EXPECT_STACK(code, expected);
}

TEST(push_pull_tests, push_3) {
	signed char code[] { PUSH_CH(10), PUSH_CH(20), PUSH_CH(30) };
	signed char expected[] { 30, 20, 10 };
	EXPECT_STACK(code, expected);
}

TEST(push_pull_tests, no_ram) {
	signed char code[] { PUSH_INT(10) };
	EXPECT_STACK_OVERFLOW(code, int_size - 1);
}
