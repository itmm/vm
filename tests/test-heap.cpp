#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(heap_tests, small_alloc_all) {
	signed char code[] { PUSH_CH(10), op_small_new };
	signed char expected[] { RAW_INT(int_size) };
	EXPECT_LIMITED_STACK(code, 10 + 2 * int_size, expected);
	EXPECT_EQ(heap_end(), stack_begin());
}

TEST(heap_tests, alloc_all) {
	signed char code[] { PUSH_SMALL_INT(20), op_new };
	signed char expected[] { RAW_INT(int_size) };
	EXPECT_LIMITED_STACK(code, 20 + 2 * int_size, expected);
	EXPECT_EQ(heap_end(), stack_begin());
}

TEST(heap_tests, simple_alloc_and_free) {
	signed char code[] { PUSH_CH(10), op_small_new, op_free };
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, 10 + 2 * int_size, expected);
	EXPECT_EQ(heap_end(), ram_begin());
}