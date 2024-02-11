#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(jmp_tests, small_jmp) {
	signed char code[] { op_small_jmp, 2, PUSH_CH(10) };
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, 1, expected);
}

TEST(jmp_tests, jmp) {
	signed char code[] { op_jmp, RAW_INT(2), PUSH_CH(10) };
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, 1, expected);
}
