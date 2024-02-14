#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(heap_tests, small_alloc_all) {
	signed char code[] { PUSH_CH(10), op_small_new };
	signed char expected[] { RAW_INT(int_size) };
	EXPECT_LIMITED_STACK(code, 1 * (10 + int_size) + int_size, expected);
	EXPECT_EQ(heap_end(), stack_begin());
}

TEST(heap_tests, alloc_all) {
	signed char code[] { PUSH_SMALL_INT(20), op_new };
	signed char expected[] { RAW_INT(int_size) };
	EXPECT_LIMITED_STACK(code, 1 * (20 + int_size) + int_size, expected);
	EXPECT_EQ(heap_end(), stack_begin());
}

TEST(heap_tests, simple_alloc_and_free) {
	signed char code[] { PUSH_CH(10), op_small_new, op_free };
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, 1 * (10 + int_size) + int_size, expected);
	EXPECT_EQ(heap_end(), ram_begin());
}

TEST(heap_tests, free_list) {
	signed char code[] {
		PUSH_CH(10), op_small_new, PUSH_CH(10), op_small_new,
		op_swap_int, op_free, PUSH_CH(10), op_small_new
	};
	signed char expected[] { RAW_INT(int_size), RAW_INT(2 * int_size + 10) };
	auto stack_size { sizeof(expected) };
	EXPECT_LIMITED_STACK(code, 2 * (10 + int_size) + stack_size, expected);
	EXPECT_EQ(heap_end(), stack_begin());
}

TEST(heap_tests, fragmentation) {
	signed char code[] {
		PUSH_CH(10), op_small_new, PUSH_CH(10), op_small_new,
		op_swap_int, op_free, PUSH_CH(12), op_small_new
	};
	EXPECT_LIMITED_STACK_ERROR(
		code, 10 + int_size + 12 + int_size + 2 * int_size,
		Error::err_heap_overflow
	);
}