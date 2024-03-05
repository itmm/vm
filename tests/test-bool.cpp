#include "gtest/gtest.h"

#include "ops/and.h"
#include "ops/or.h"
#include "ops/xor.h"
#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	TEST(bool_tests, not_true_ch) {
		signed char code[] { PUSH_BYTE(true_lit), op_not };
		signed char expected[] { RAW_BYTE(false_lit) };
		EXPECT_LIMITED_STACK(code, Byte::typed_size, expected);
	}

	TEST(bool_tests, not_false_ch) {
		signed char code[] { PUSH_BYTE(false_lit), op_not };
		signed char expected[] { RAW_BYTE(true_lit) };
		EXPECT_LIMITED_STACK(code, Byte::typed_size, expected);
	}

	TEST(bool_tests, not_underflow_ch) {
		signed char code[] { op_not };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif

#if CONFIG_WITH_INT
	TEST(bool_tests, not_true_int) {
		signed char code[] { PUSH_SMALL_INT(true_lit), op_not };
		signed char expected[] { RAW_INT(false_lit) };
		EXPECT_LIMITED_STACK(code, Int::typed_size, expected);
	}

	TEST(bool_tests, not_false_int) {
		signed char code[] { PUSH_SMALL_INT(false_lit), op_not };
		signed char expected[] { RAW_INT(true_lit) };
		EXPECT_LIMITED_STACK(code, Int::typed_size, expected);
	}
#endif

#if CONFIG_WITH_BYTE
	TEST(bool_tests, and00_ch) {
		signed char code[] { PUSH_BYTE(false_lit), PUSH_BYTE(false_lit), op_and };
		signed char expected[] { RAW_BYTE(false_lit) };
		EXPECT_LIMITED_STACK(code, 2 * Byte::typed_size, expected);
	}

	TEST(bool_tests, and01_ch) {
		EXPECT_EQ(
			ops::And { } (Byte { false_lit }, Byte { true_lit }),
			Value { Byte { false_lit }}
		);
	}

	TEST(bool_tests, and10_ch) {
		EXPECT_EQ(
			ops::And { } (Byte { true_lit }, Byte { false_lit }),
			Value { Byte { false_lit }}
		);
	}

	TEST(bool_tests, and11_ch) {
		EXPECT_EQ(
			ops::And { } (Byte { true_lit }, Byte { true_lit }),
			Value { Byte { true_lit }}
		);
	}

	TEST(bool_tests, and_underflow_ch) {
		signed char code[] { PUSH_BYTE(true_lit), op_and };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif

#if CONFIG_WITH_INT
	TEST(bool_tests, and00_int) {
		EXPECT_EQ(
			ops::And { } (Int { false_lit }, Int { false_lit }),
			Value { Int { false_lit }}
		);
	}

	TEST(bool_tests, and01_int) {
		EXPECT_EQ(
			ops::And { } (Int { false_lit }, Int { true_lit }),
			Value { Int { false_lit }}
		);
	}

	TEST(bool_tests, and10_int) {
		EXPECT_EQ(
			ops::And { } (Int { false_lit }, Int { true_lit }),
			Value { Int { false_lit }}
		);
	}

	TEST(bool_tests, and11_int) {
		EXPECT_EQ(
			ops::And { } (Int { true_lit }, Int { true_lit }),
			Value { Int { true_lit }}
		);
	}

	TEST(bool_tests, and_underflow_int) {
		signed char code[] { PUSH_SMALL_INT(true_lit), op_and };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif

#if CONFIG_WITH_BYTE
	TEST(bool_tests, or00_ch) {
		signed char code[] { PUSH_BYTE(false_lit), PUSH_BYTE(false_lit), op_or };
		signed char expected[] { RAW_BYTE(false_lit) };
		EXPECT_LIMITED_STACK(code, 2 * Byte::typed_size, expected);
	}

	TEST(bool_tests, or01_ch) {
		EXPECT_EQ(
			ops::Or { } (Byte { false_lit }, Byte { true_lit }),
			Value { Byte { true_lit } }
		);
	}

	TEST(bool_tests, or10_ch) {
		EXPECT_EQ(
			ops::Or { } (Byte { true_lit }, Byte { false_lit }),
			Value { Byte { true_lit } }
		);
	}

	TEST(bool_tests, or11_ch) {
		EXPECT_EQ(
			ops::Or { } (Byte { true_lit }, Byte { true_lit }),
			Value { Byte { true_lit } }
		);
	}

	TEST(bool_tests, or_underflow_ch) {
		signed char code[] { PUSH_BYTE(true_lit), op_or };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif

#if CONFIG_WITH_INT
	TEST(bool_tests, or00_int) {
		EXPECT_EQ(
			ops::Or { } (Int { false_lit }, Int { false_lit }),
			Value { Int { false_lit } }
		);
	}

	TEST(bool_tests, or01_int) {
		EXPECT_EQ(
			ops::Or { } (Int { false_lit }, Int { true_lit }),
			Value { Int { true_lit } }
		);
	}

	TEST(bool_tests, or10_int) {
		EXPECT_EQ(
			ops::Or { } (Int { true_lit }, Int { false_lit }),
			Value { Int { true_lit } }
		);
	}

	TEST(bool_tests, or11_int) {
		EXPECT_EQ(
			ops::Or { } (Int { true_lit }, Int { true_lit }),
			Value { Int { true_lit } }
		);
	}

	TEST(bool_tests, or_underflow_int) {
		signed char code[] { PUSH_SMALL_INT(true_lit), op_or };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif

#if CONFIG_WITH_BYTE
	TEST(bool_tests, xor00_ch) {
		signed char code[] { PUSH_BYTE(false_lit), PUSH_BYTE(false_lit), op_xor };
		signed char expected[] { RAW_BYTE(false_lit) };
		EXPECT_LIMITED_STACK(code, 2 * Byte::typed_size, expected);
	}

	TEST(bool_tests, xor01_ch) {
		EXPECT_EQ(
			ops::Xor { } (Byte { false_lit }, Byte { true_lit }),
			Value { Byte { true_lit }}
		);
	}

	TEST(bool_tests, xor10_ch) {
		EXPECT_EQ(
			ops::Xor { } (Byte { true_lit }, Byte { false_lit }),
			Value { Byte { true_lit }}
		);
	}

	TEST(bool_tests, xor11_ch) {
		EXPECT_EQ(
			ops::Xor { } (Byte { true_lit }, Byte { true_lit }),
			Value { Byte { false_lit }}
		);
	}

	TEST(bool_tests, xor_underflow_ch) {
		signed char code[] { PUSH_BYTE(true_lit), op_xor };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif

#if CONFIG_WITH_INT
	TEST(bool_tests, xor00_int) {
		EXPECT_EQ(
			ops::Xor { } (Int { false_lit }, Int { false_lit }),
			Value { Int { false_lit }}
		);
	}

	TEST(bool_tests, xor01_int) {
		EXPECT_EQ(
			ops::Xor { } (Int { false_lit }, Int { true_lit }),
			Value { Int { true_lit }}
		);
	}

	TEST(bool_tests, xor10_int) {
		EXPECT_EQ(
			ops::Xor { } (Int { true_lit }, Int { false_lit }),
			Value { Int { true_lit }}
		);
	}

	TEST(bool_tests, xor11_int) {
		EXPECT_EQ(
			ops::Xor { } (Int { true_lit }, Int { true_lit }),
			Value { Int { false_lit }}
		);
	}

	TEST(bool_tests, xor_underflow_int) {
		signed char code[] { PUSH_SMALL_INT(true_lit), op_xor };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif
