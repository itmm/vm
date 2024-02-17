#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(swap_tests, simple) {
	signed char code[] { PUSH_SMALL_INT(1), PUSH_SMALL_INT(2), op_swap_int };
	signed char expected[] { RAW_INT(1), RAW_INT(2) };
	EXPECT_LIMITED_STACK(code, 2 * int_size, expected);
}

TEST(swap_tests, empty_ch) {
	signed char code[] { PUSH_SMALL_INT(1), PUSH_CH(0), op_swap_int };
	EXPECT_ERROR(code, Error::err_no_integer);
}
