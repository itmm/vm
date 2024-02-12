#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(store_tests, small_store_ch) {
	signed char code[] {
		PUSH_CH(10), PUSH_CH(11), PUSH_CH(20), PUSH_CH(ch_size),
		op_small_store_ch
	};
	signed char expected[] { 11, 20 };
	EXPECT_LIMITED_STACK(code, 4 * ch_size, expected);
}

TEST(store_tests, store_ch) {
	signed char code[] {
		PUSH_CH(10), PUSH_CH(11), PUSH_CH(20), PUSH_SMALL_INT(ch_size),
		op_store_ch
	};
	signed char expected[] { 11, 20 };
	EXPECT_LIMITED_STACK(code, 3 * ch_size + int_size, expected);
}
TEST(store_tests, small_store_int) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(11), PUSH_SMALL_INT(20),
		PUSH_CH(int_size), op_small_store_int
	};
	signed char expected[] { RAW_INT(11), RAW_INT(20) };
	EXPECT_LIMITED_STACK(code, ch_size + 3 * int_size, expected);
}

TEST(store_tests, store_int) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(11), PUSH_SMALL_INT(20),
		PUSH_SMALL_INT(4), op_store_int
	};
	signed char expected[] { RAW_INT(11), RAW_INT(20) };
	EXPECT_LIMITED_STACK(code, 4 * int_size, expected);
}
