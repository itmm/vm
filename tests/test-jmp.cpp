#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_CHAR
	TEST(jmp_tests, small_jmp) {
		signed char code[] { op_near_jmp, Char::raw_size + 1, PUSH_CH(10) };
		signed char expected[] { };
		EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
	}

	TEST(jmp_tests, small_jmp_true) {
		signed char code[] {
			PUSH_CH(true_lit), op_near_jmp_true, Char::raw_size + 1, PUSH_CH(10)
		};
		signed char expected[] { };
		EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
	}

	TEST(jmp_tests, dont_small_jmp_true) {
		signed char code[] {
			PUSH_CH(false_lit), op_near_jmp_true, Char::raw_size + 1, PUSH_CH(10)
		};
		signed char expected[] { RAW_CH(10) };
		EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
	}

	TEST(jmp_tests, small_jmp_false) {
		signed char code[] {
			PUSH_CH(false_lit), op_near_jmp_false, Char::raw_size + 1,
			PUSH_CH(10)
		};
		signed char expected[] { };
		EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
	}

	TEST(jmp_tests, dont_small_jmp_false) {
		signed char code[] {
			PUSH_CH(true_lit), op_near_jmp_false, Char::raw_size + 1,
			PUSH_CH(10)
		};
		signed char expected[] { RAW_CH(10) };
		EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
	}
#endif

#if CONFIG_WITH_INT
	TEST(jmp_tests, jmp) {
		signed char code[] { op_jmp, RAW_INT_(Int::raw_size + 1), PUSH_INT(10) };
		signed char expected[] { };
		EXPECT_LIMITED_STACK(code, Char::typed_size, expected);
	}

	TEST(jmp_tests, jmp_true) {
		signed char code[] {
			PUSH_INT(true_lit), op_jmp_true, RAW_INT_(Int::raw_size + 1),
			PUSH_INT(10)
		};
		signed char expected[] { };
		EXPECT_LIMITED_STACK(code, Int::typed_size, expected);
	}

	TEST(jmp_tests, dont_jmp_true) {
		signed char code[] {
			PUSH_INT(false_lit), op_jmp_true, RAW_INT_(Int::raw_size + 1),
			PUSH_INT(10)
		};
		signed char expected[] { RAW_INT(10) };
		EXPECT_LIMITED_STACK(code, Int::typed_size, expected);
	}

	TEST(jmp_tests, jmp_false) {
		signed char code[] {
			PUSH_INT(false_lit), op_jmp_false, RAW_INT_(Int::raw_size + 1),
			PUSH_INT(10)
		};
		signed char expected[] { };
		EXPECT_LIMITED_STACK(code, Int::typed_size, expected);
	}

	TEST(jmp_tests, dont_jmp_false) {
		signed char code[] {
			PUSH_INT(true_lit), op_jmp_false, RAW_INT_(Int::raw_size + 1),
			PUSH_INT(10)
		};
		signed char expected[] { RAW_INT(10) };
		EXPECT_LIMITED_STACK(code, Int::typed_size, expected);
	}
#endif

