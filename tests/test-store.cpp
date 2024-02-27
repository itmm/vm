#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(store_tests, small_store_ch) {
	signed char code[] {
		PUSH_CH(10), PUSH_CH(11), PUSH_CH(20), PUSH_CH(Char::typed_size), op_store
	};
	signed char expected[] { RAW_CH(11), RAW_CH(20) };
	EXPECT_LIMITED_STACK(code, 4 * Char::typed_size, expected);
}

TEST(store_tests, store_ch) {
	signed char code[] {
		PUSH_CH(10), PUSH_CH(11), PUSH_CH(20), PUSH_SMALL_INT(Char::typed_size), op_store
	};
	signed char expected[] { RAW_CH(11), RAW_CH(20) };
	EXPECT_LIMITED_STACK(code, 3 * Char::typed_size + int_size, expected);
}
TEST(store_tests, small_store_int) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(11), PUSH_SMALL_INT(20),
		PUSH_CH(int_size), op_store
	};
	signed char expected[] { RAW_INT(11), RAW_INT(20) };
	EXPECT_LIMITED_STACK(code, Char::typed_size + 3 * int_size, expected);
}

TEST(store_tests, store_int) {
	signed char code[] {
		PUSH_SMALL_INT(10), PUSH_SMALL_INT(11), PUSH_SMALL_INT(20),
		PUSH_SMALL_INT(int_size), op_store
	};
	signed char expected[] { RAW_INT(11), RAW_INT(20) };
	EXPECT_LIMITED_STACK(code, 4 * int_size, expected);
}
