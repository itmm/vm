#include "gtest/gtest.h"

#include "ops/sub.h"
#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_INT
	TEST(sub_tests, simple) {
		signed char code[] { PUSH_SMALL_INT(30), PUSH_SMALL_INT(20), op_sub };
		signed char expected[] { RAW_INT(10) };
		EXPECT_STACK(code, expected);
	}

	TEST(sub_tests, negative) {
		EXPECT_EQ(ops::Sub { } (Int { -10 }, Int { 20 }), Value { Int { -30 }});
	}

	TEST(sub_tests, big_ok_a) {
		EXPECT_EQ(
			ops::Sub { } (Int { 0x7ffffffd }, Int { -2 }),
			Value { Int { 0x7fffffff } }
		);
	}

	TEST(sub_tests, big_overflow_a) {
		signed char code[] { PUSH_INT(0x7ffffffd), PUSH_SMALL_INT(-3), op_sub };
		EXPECT_ERROR(code, Err::sub_overflow);
	}

	TEST(sub_tests, big_ok_b) {
		EXPECT_EQ(
			ops::Sub { } (Int { 2 }, Int { -0x7ffffffd }),
			Value { Int { 0x7fffffff } }
		);
	}

	TEST(sub_tests, big_overflow_b) {
		signed char code[] { PUSH_SMALL_INT(3), PUSH_INT(-0x7ffffffd), op_sub };
		EXPECT_ERROR(code, Err::sub_overflow);
	}

	TEST(sub_tests, two_bigs) {
		EXPECT_EQ(
			ops::Sub { } (Int { 0x7fffffff }, Int { 0x7ffffffd }),
			Value { Int { 2} }
		);
	}

	TEST(sub_tests, underflow) {
		signed char code[] { PUSH_SMALL_INT(5), op_sub };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif
