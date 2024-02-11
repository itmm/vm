#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(bool_tests, not_true_ch) {
	signed char code[] { PUSH_CH(true_lit), op_not_ch };
	signed char expected[] { false_lit };
	EXPECT_LIMITED_STACK(code, ch_size, expected);
}

TEST(bool_tests, not_false_ch) {
	signed char code[] { PUSH_CH(false_lit), op_not_ch };
	signed char expected[] { true_lit };
	EXPECT_LIMITED_STACK(code, ch_size, expected);
}

TEST(bool_tests, not_underflow_ch) {
	signed char code[] { op_not_ch };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(bool_tests, not_true_int) {
	signed char code[] { PUSH_SMALL_INT(true_lit), op_not_int };
	signed char expected[] { RAW_INT(false_lit) };
	EXPECT_LIMITED_STACK(code, int_size, expected);
}

TEST(bool_tests, not_false_int) {
	signed char code[] { PUSH_SMALL_INT(false_lit), op_not_int };
	signed char expected[] { RAW_INT(true_lit) };
	EXPECT_LIMITED_STACK(code, int_size, expected);
}

TEST(bool_tests, not_underflow_int) {
	signed char code[] { PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), op_not_int };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(bool_tests, and00_ch) {
	signed char code[] { PUSH_CH(false_lit), PUSH_CH(false_lit), op_and_ch };
	signed char expected[] { false_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, and01_ch) {
	signed char code[] { PUSH_CH(true_lit), PUSH_CH(false_lit), op_and_ch };
	signed char expected[] { false_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, and10_ch) {
	signed char code[] { PUSH_CH(false_lit), PUSH_CH(true_lit), op_and_ch };
	signed char expected[] { false_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, and11_ch) {
	signed char code[] { PUSH_CH(true_lit), PUSH_CH(true_lit), op_and_ch };
	signed char expected[] { true_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, and_underflow_ch) {
	signed char code[] { PUSH_CH(true_lit), op_and_ch };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(bool_tests, and00_int) {
	signed char code[] {
		PUSH_SMALL_INT(false_lit), PUSH_SMALL_INT(false_lit), op_and_int
	};
	signed char expected[] { RAW_INT(false_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, and01_int) {
	signed char code[] {
		PUSH_SMALL_INT(true_lit), PUSH_SMALL_INT(false_lit), op_and_int
	};
	signed char expected[] { RAW_INT(false_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, and10_int) {
	signed char code[] {
		PUSH_SMALL_INT(false_lit), PUSH_SMALL_INT(true_lit), op_and_int
	};
	signed char expected[] { RAW_INT(false_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, and11_int) {
	signed char code[] {
		PUSH_SMALL_INT(true_lit), PUSH_SMALL_INT(true_lit), op_and_int
	};
	signed char expected[] { RAW_INT(true_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, and_underflow_int) {
	signed char code[] {
		PUSH_SMALL_INT(true_lit), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), op_and_int
	};
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(bool_tests, or00_ch) {
	signed char code[] { PUSH_CH(false_lit), PUSH_CH(false_lit), op_or_ch };
	signed char expected[] { false_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, or01_ch) {
	signed char code[] { PUSH_CH(true_lit), PUSH_CH(false_lit), op_or_ch };
	signed char expected[] { true_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, or10_ch) {
	signed char code[] { PUSH_CH(false_lit), PUSH_CH(true_lit), op_or_ch };
	signed char expected[] { true_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, or11_ch) {
	signed char code[] { PUSH_CH(true_lit), PUSH_CH(true_lit), op_or_ch };
	signed char expected[] { true_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, or_underflow_ch) {
	signed char code[] { PUSH_CH(true_lit), op_or_ch };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(bool_tests, or00_int) {
	signed char code[] {
		PUSH_SMALL_INT(false_lit), PUSH_SMALL_INT(false_lit), op_or_int
	};
	signed char expected[] { RAW_INT(false_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, or01_int) {
	signed char code[] {
		PUSH_SMALL_INT(true_lit), PUSH_SMALL_INT(false_lit), op_or_int
	};
	signed char expected[] { RAW_INT(true_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, or10_int) {
	signed char code[] {
		PUSH_SMALL_INT(false_lit), PUSH_SMALL_INT(true_lit), op_or_int
	};
	signed char expected[] { RAW_INT(true_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, or11_int) {
	signed char code[] {
		PUSH_SMALL_INT(true_lit), PUSH_SMALL_INT(true_lit), op_or_int
	};
	signed char expected[] { RAW_INT(true_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, or_underflow_int) {
	signed char code[] {
		PUSH_SMALL_INT(true_lit), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), op_or_int
	};
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(bool_tests, xor00_ch) {
	signed char code[] { PUSH_CH(false_lit), PUSH_CH(false_lit), op_xor_ch };
	signed char expected[] { false_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, xor01_ch) {
	signed char code[] { PUSH_CH(true_lit), PUSH_CH(false_lit), op_xor_ch };
	signed char expected[] { true_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, xor10_ch) {
	signed char code[] { PUSH_CH(false_lit), PUSH_CH(true_lit), op_xor_ch };
	signed char expected[] { true_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, xor11_ch) {
	signed char code[] { PUSH_CH(true_lit), PUSH_CH(true_lit), op_xor_ch };
	signed char expected[] { false_lit };
	EXPECT_LIMITED_STACK(code, 2 * ch_size, expected);
}

TEST(bool_tests, xor_underflow_ch) {
	signed char code[] { PUSH_CH(true_lit), op_xor_ch };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(bool_tests, xor00_int) {
	signed char code[] {
		PUSH_SMALL_INT(false_lit), PUSH_SMALL_INT(false_lit), op_xor_int
	};
	signed char expected[] { RAW_INT(false_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, xor01_int) {
	signed char code[] {
		PUSH_SMALL_INT(true_lit), PUSH_SMALL_INT(false_lit), op_xor_int
	};
	signed char expected[] { RAW_INT(true_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, xor10_int) {
	signed char code[] {
		PUSH_SMALL_INT(false_lit), PUSH_SMALL_INT(true_lit), op_xor_int
	};
	signed char expected[] { RAW_INT(true_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, xor11_int) {
	signed char code[] {
		PUSH_SMALL_INT(true_lit), PUSH_SMALL_INT(true_lit), op_xor_int
	};
	signed char expected[] { RAW_INT(false_lit) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(bool_tests, xor_underflow_int) {
	signed char code[] {
		PUSH_SMALL_INT(true_lit), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), op_xor_int
	};
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}
