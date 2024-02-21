#include "gtest/gtest.h"

#include "vm-tests.h"
#include "ptr.h"

using namespace vm;

TEST(heap_tests, small_alloc_all) {
	signed char code[] { PUSH_CH(10), op_new };
	signed char expected[] { RAW_PTR(heap_overhead) };
	EXPECT_LIMITED_STACK(code, 10 + heap_overhead + ptr_size, expected);
	EXPECT_EQ(heap_end, stack_begin);
}

TEST(heap_tests, alloc_all) {
	signed char code[] { PUSH_SMALL_INT(20), op_new };
	signed char expected[] { RAW_PTR(heap_overhead) };
	EXPECT_LIMITED_STACK(code, 20 + heap_overhead + ptr_size, expected);
	EXPECT_EQ(heap_end, stack_begin);
}

TEST(heap_tests, simple_alloc_and_free) {
	signed char code[] { PUSH_CH(10), op_new, op_free };
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, 10 + heap_overhead + ptr_size, expected);
	EXPECT_EQ(heap_end, ram_begin);
}

TEST(heap_tests, free_list) {
	signed char code[] {
		PUSH_CH(10), op_new, PUSH_CH(10), op_new,
		op_swap, op_free, PUSH_CH(10), op_new
	};
	int heap_size { 2 * (10 + heap_overhead) };
	signed char expected[] {
		RAW_PTR(heap_overhead), RAW_PTR(2 * heap_overhead + 10)
	};
	auto stack_size { sizeof(expected) };
	Enable_Dump enable_dump { true, true, true };
	EXPECT_LIMITED_STACK(code, heap_size + stack_size, expected);
	EXPECT_EQ(heap_end, stack_begin);
}


TEST(heap_tests, fragmentation) {
	signed char code[] {
		PUSH_CH(10), op_new, PUSH_CH(10), op_new,
		op_swap, op_free, PUSH_CH(12), op_new
	};
	EXPECT_LIMITED_STACK_ERROR(
		code, 10 + heap_overhead + 12 + heap_overhead + 2 * int_size,
		Err::heap_overflow
	);
}
