#include "gtest/gtest.h"

#include "vm-tests.h"

TEST(bool_tests, not_true_ch) {
	signed char code[] { PUSH_CH(-1), vm::op_not_ch };
	signed char expected[] { 0 };
	EXPECT_LIMITED_STACK(code, 1, expected);
}

TEST(bool_tests, not_false_ch) {
	signed char code[] { PUSH_CH(0), vm::op_not_ch };
	signed char expected[] { -1 };
	EXPECT_LIMITED_STACK(code, 1, expected);
}

TEST(bool_tests, not_underflow_ch) {
	signed char code[] { vm::op_not_ch };
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}

TEST(bool_tests, not_true_int) {
	signed char code[] { PUSH_SMALL_INT(-1), vm::op_not_int };
	signed char expected[] { RAW_INT(0) };
	EXPECT_LIMITED_STACK(code, 4, expected);
}

TEST(bool_tests, not_false_int) {
	signed char code[] { PUSH_SMALL_INT(0), vm::op_not_int };
	signed char expected[] { RAW_INT(-1) };
	EXPECT_LIMITED_STACK(code, 4, expected);
}

TEST(bool_tests, not_underflow_int) {
	signed char code[] { PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), vm::op_not_int };
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}

TEST(bool_tests, and00_ch) {
	signed char code[] { PUSH_CH(0), PUSH_CH(0), vm::op_and_ch };
	signed char expected[] { 0 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, and01_ch) {
	signed char code[] { PUSH_CH(-1), PUSH_CH(0), vm::op_and_ch };
	signed char expected[] { 0 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, and10_ch) {
	signed char code[] { PUSH_CH(0), PUSH_CH(-1), vm::op_and_ch };
	signed char expected[] { 0 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, and11_ch) {
	signed char code[] { PUSH_CH(-1), PUSH_CH(-1), vm::op_and_ch };
	signed char expected[] { -1 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, and_underflow_ch) {
	signed char code[] { PUSH_CH(-1), vm::op_and_ch };
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}

TEST(bool_tests, and00_int) {
	signed char code[] { PUSH_SMALL_INT(0), PUSH_SMALL_INT(0), vm::op_and_int };
	signed char expected[] { RAW_INT(0) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, and01_int) {
	signed char code[] {
		PUSH_SMALL_INT(-1), PUSH_SMALL_INT(0), vm::op_and_int
	};
	signed char expected[] { RAW_INT(0) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, and10_int) {
	signed char code[] {
		PUSH_SMALL_INT(0), PUSH_SMALL_INT(-1), vm::op_and_int
	};
	signed char expected[] { RAW_INT(0) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, and11_int) {
	signed char code[] {
		PUSH_SMALL_INT(-1), PUSH_SMALL_INT(-1), vm::op_and_int
	};
	signed char expected[] { RAW_INT(-1) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, and_underflow_int) {
	signed char code[] {
		PUSH_SMALL_INT(-1), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), vm::op_and_int
	};
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}

TEST(bool_tests, or00_ch) {
	signed char code[] { PUSH_CH(0), PUSH_CH(0), vm::op_or_ch };
	signed char expected[] { 0 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, or01_ch) {
	signed char code[] { PUSH_CH(-1), PUSH_CH(0), vm::op_or_ch };
	signed char expected[] { -1 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, or10_ch) {
	signed char code[] { PUSH_CH(0), PUSH_CH(-1), vm::op_or_ch };
	signed char expected[] { -1 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, or11_ch) {
	signed char code[] { PUSH_CH(-1), PUSH_CH(-1), vm::op_or_ch };
	signed char expected[] { -1 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, or_underflow_ch) {
	signed char code[] { PUSH_CH(-1), vm::op_or_ch };
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}

TEST(bool_tests, or00_int) {
	signed char code[] { PUSH_SMALL_INT(0), PUSH_SMALL_INT(0), vm::op_or_int };
	signed char expected[] { RAW_INT(0) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, or01_int) {
	signed char code[] { PUSH_SMALL_INT(-1), PUSH_SMALL_INT(0), vm::op_or_int };
	signed char expected[] { RAW_INT(-1) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, or10_int) {
	signed char code[] { PUSH_SMALL_INT(0), PUSH_SMALL_INT(-1), vm::op_or_int };
	signed char expected[] { RAW_INT(-1) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, or11_int) {
	signed char code[] {
		PUSH_SMALL_INT(-1), PUSH_SMALL_INT(-1), vm::op_or_int
	};
	signed char expected[] { RAW_INT(-1) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, or_underflow_int) {
	signed char code[] {
		PUSH_SMALL_INT(-1), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), vm::op_or_int
	};
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}

TEST(bool_tests, xor00_ch) {
	signed char code[] { PUSH_CH(0), PUSH_CH(0), vm::op_xor_ch };
	signed char expected[] { 0 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, xor01_ch) {
	signed char code[] { PUSH_CH(-1), PUSH_CH(0), vm::op_xor_ch };
	signed char expected[] { -1 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, xor10_ch) {
	signed char code[] { PUSH_CH(0), PUSH_CH(-1), vm::op_xor_ch };
	signed char expected[] { -1 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, xor11_ch) {
	signed char code[] { PUSH_CH(-1), PUSH_CH(-1), vm::op_xor_ch };
	signed char expected[] { 0 };
	EXPECT_LIMITED_STACK(code, 2, expected);
}

TEST(bool_tests, xor_underflow_ch) {
	signed char code[] { PUSH_CH(-1), vm::op_xor_ch };
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}

TEST(bool_tests, xor00_int) {
	signed char code[] { PUSH_SMALL_INT(0), PUSH_SMALL_INT(0), vm::op_xor_int };
	signed char expected[] { RAW_INT(0) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, xor01_int) {
	signed char code[] {
		PUSH_SMALL_INT(-1), PUSH_SMALL_INT(0), vm::op_xor_int
	};
	signed char expected[] { RAW_INT(-1) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, xor10_int) {
	signed char code[] {
		PUSH_SMALL_INT(0), PUSH_SMALL_INT(-1), vm::op_xor_int
	};
	signed char expected[] { RAW_INT(-1) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, xor11_int) {
	signed char code[] {
		PUSH_SMALL_INT(-1), PUSH_SMALL_INT(-1), vm::op_xor_int
	};
	signed char expected[] { RAW_INT(0) };
	EXPECT_LIMITED_STACK(code, 8, expected);
}

TEST(bool_tests, xor_underflow_int) {
	signed char code[] {
		PUSH_SMALL_INT(-1), PUSH_CH(0), PUSH_CH(0), PUSH_CH(0), vm::op_xor_int
	};
	EXPECT_ERROR(code, vm::Error::err_stack_underflow);
}
