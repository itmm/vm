#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(fetch_tests, fetch_ch) {
	signed char code[] {
		PUSH_CH(10), PUSH_CH(20), PUSH_SMALL_INT(ch_size), op_fetch_ch
	};
	signed char expected[] { RAW_CH(10), RAW_CH(20), RAW_CH(10) };
	EXPECT_LIMITED_STACK(code, 2 * ch_size + int_size, expected);
}

TEST(fetch_tests, small_fetch_ch) {
	signed char code[] {
		PUSH_CH(10), PUSH_CH(20), PUSH_CH(ch_size), op_small_fetch_ch
	};
	signed char expected[] { RAW_CH(10), RAW_CH(20), RAW_CH(10) };
	EXPECT_LIMITED_STACK(code, 3 * ch_size, expected);
}

TEST(fetch_tests, ch_out_of_range) {
	signed char code[] { PUSH_CH(10), PUSH_SMALL_INT(ch_size), op_fetch_ch };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}

TEST(fetch_tests, fetch_int) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(20),
		PUSH_SMALL_INT(int_size), op_fetch_int
	};
	signed char expected[] { RAW_INT(10), RAW_INT(20), RAW_INT(10) };
	EXPECT_LIMITED_STACK(code, 3 * int_size, expected);
}

TEST(fetch_tests, small_fetch_int) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(20),
		PUSH_CH(int_size), op_small_fetch_int
	};
	signed char expected[] { RAW_INT(10), RAW_INT(20), RAW_INT(10) };
	EXPECT_LIMITED_STACK(code, 3 * int_size, expected);
}

TEST(fetch_tests, int_out_of_range) {
	signed char code[] { PUSH_SMALL_INT(10), PUSH_SMALL_INT(4), op_fetch_int };
	EXPECT_ERROR(code, Error::err_leave_stack_segment);
}