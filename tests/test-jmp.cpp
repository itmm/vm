#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(jmp_tests, small_jmp) {
	signed char code[] { op_near_jmp, 2, PUSH_CH(10) };
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
}

TEST(jmp_tests, jmp) {
	signed char code[] { op_jmp, RAW_INT_(2), PUSH_CH(10) };
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
}

TEST(jmp_tests, small_jmp_true) {
	signed char code[] { PUSH_CH(true_lit), op_near_jmp_true, 2, PUSH_CH(10) };
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
}

TEST(jmp_tests, dont_small_jmp_true) {
	signed char code[] {
		PUSH_CH(false_lit), op_near_jmp_true, 2, PUSH_CH(10)
	};
	signed char expected[] { RAW_CH(10) };
	EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
}

TEST(jmp_tests, jmp_true) {
	signed char code[] {
		PUSH_CH(true_lit), op_jmp_true, RAW_INT_(2), PUSH_CH(10)
	};
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
}

TEST(jmp_tests, dont_jmp_true) {
	signed char code[] {
		PUSH_CH(false_lit), op_jmp_true, RAW_INT_(2), PUSH_CH(10)
	};
	signed char expected[] { RAW_CH(10) };
	EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
}

TEST(jmp_tests, small_jmp_false) {
	signed char code[] {
		PUSH_CH(false_lit), op_near_jmp_false, 2, PUSH_CH(10)
	};
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
}

TEST(jmp_tests, dont_small_jmp_false) {
	signed char code[] {
		PUSH_CH(true_lit), op_near_jmp_false, 2, PUSH_CH(10)
	};
	signed char expected[] { RAW_CH(10) };
	EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
}

TEST(jmp_tests, jmp_false) {
	signed char code[] {
		PUSH_CH(false_lit), op_jmp_false, RAW_INT_(2), PUSH_CH(10)
	};
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
}

TEST(jmp_tests, dont_jmp_false) {
	signed char code[] {
		PUSH_CH(true_lit), op_jmp_false, RAW_INT_(2), PUSH_CH(10)
	};
	signed char expected[] { RAW_CH(10) };
	EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
}
