#include "gtest/gtest.h"

#include "ops/mod.h"
#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_INT
	TEST(mod_tests, simple) {
		signed char code[] { PUSH_SMALL_INT(23), PUSH_SMALL_INT(5), op_mod };
		signed char expected[] { RAW_INT(3) };
		EXPECT_STACK(code, expected);
	}

	TEST(mod_tests, exact) {
		EXPECT_EQ(ops::Mod { } (Int { 20 }, Int { 4 }), Value { Int { 0 } });
	}

	TEST(mod_tests, negative) {
		#if CONFIG_OBERON_MATH
			Int expected { 2 };
		#else
			Int expected { -3 };
		#endif
		EXPECT_EQ(ops::Mod { } (Int { -23 }, Int { 5 }), Value { expected });
	}

	TEST(mod_tests, negative_exact) {
		EXPECT_EQ(ops::Mod { } (Int { -20 }, Int { 5 }), Value { Int { 0 } });
	}

	TEST(mod_tests, big_positive) {
		EXPECT_EQ(
			ops::Mod { } (Int { 0x7fffffff }, Int { 2 }), Value { Int { 1 } }
		);
	}

	TEST(mod_tests, big_negative) {
		Int big_neg { static_cast<int>(0x80000000) };
		EXPECT_EQ(ops::Mod { } (big_neg, Int { 2 }), Value { Int { 0 } });
	}

	TEST(mod_tests, overflow) {
		signed char code[] { PUSH_SMALL_INT(0), PUSH_INT(0x80000000), op_mod };
		signed char expected[] { RAW_INT(0) };
		EXPECT_STACK(code, expected);
	}

	TEST(mod_tests, div_by_0) {
		signed char code[] { PUSH_SMALL_INT(10), PUSH_SMALL_INT(0), op_mod };
		EXPECT_ERROR(code, Err::mod_divide_by_0);
	}

	TEST(mod_tests, underflow) {
		signed char code[] { PUSH_SMALL_INT(5), op_mod };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif