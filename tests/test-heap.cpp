#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(heap_tests, small_alloc_all) {
	signed char code[] { PUSH_CH(10), op_small_new };
	signed char expected[] { RAW_INT(heap_overhead) };
	EXPECT_LIMITED_STACK(code, 1 * (10 + heap_overhead) + int_size, expected);
	EXPECT_EQ(heap_end(), stack_begin());
}

TEST(heap_tests, alloc_all) {
	signed char code[] { PUSH_SMALL_INT(20), op_new };
	signed char expected[] { RAW_INT(heap_overhead) };
	EXPECT_LIMITED_STACK(code, 1 * (20 + heap_overhead) + int_size, expected);
	EXPECT_EQ(heap_end(), stack_begin());
}

TEST(heap_tests, simple_alloc_and_free) {
	signed char code[] { PUSH_CH(10), op_small_new, op_free };
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, 1 * (10 + heap_overhead) + int_size, expected);
	EXPECT_EQ(heap_end(), ram_begin());
}

TEST(heap_tests, free_list) {
	signed char code[] {
		PUSH_CH(10), op_small_new, PUSH_CH(10), op_small_new,
		op_swap_int, op_free, PUSH_CH(10), op_small_new
	};
	signed char expected[] { RAW_INT(heap_overhead), RAW_INT(2 * heap_overhead + 10) };
	auto stack_size { sizeof(expected) };
	EXPECT_LIMITED_STACK(code, 2 * (10 + heap_overhead) + stack_size, expected);
	EXPECT_EQ(heap_end(), stack_begin());
}


TEST(heap_tests, fragmentation) {
	signed char code[] {
		PUSH_CH(10), op_small_new, PUSH_CH(10), op_small_new,
		op_swap_int, op_free, PUSH_CH(12), op_small_new
	};
	EXPECT_LIMITED_STACK_ERROR(
		code, 10 + heap_overhead + 12 + heap_overhead + 2 * int_size,
		Error::err_heap_overflow
	);
}
