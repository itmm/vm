#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(push_pull_tests, empty) {
	signed char code[] { }, expected[] {};
	EXPECT_STACK(code, expected);
}

#if CONFIG_WITH_BYTE
	TEST(push_pull_tests, push_3) {
		signed char code[] { PUSH_BYTE(10), PUSH_BYTE(20), PUSH_BYTE(30) };
		signed char expected[] { RAW_BYTE(30), RAW_BYTE(20), RAW_BYTE(10) };
		EXPECT_STACK(code, expected);
	}
#endif

#if CONFIG_WITH_INT
	TEST(push_pull_tests, no_ram) {
		signed char code[] { PUSH_INT(10) };
		EXPECT_STACK_OVERFLOW(code, Int::typed_size - 1);
	}
#endif
