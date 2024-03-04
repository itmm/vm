#include "gtest/gtest.h"

#include "vm-tests.h"
#include "ptr.h"

using namespace vm;

#if CONFIG_WITH_HEAP
	TEST(heap_tests, small_alloc_all) {
		signed char code[] { PUSH_BYTE(10), op_new };
		signed char expected[] { RAW_PTR(heap_overhead) };
		EXPECT_LIMITED_STACK(code, 10 + heap_overhead + ptr_size, expected);
		EXPECT_EQ(Heap_Ptr::end, Stack_Ptr::begin);
	}

	TEST(heap_tests, alloc_all) {
		signed char code[] { PUSH_SMALL_INT(20), op_new };
		signed char expected[] { RAW_PTR(heap_overhead) };
		EXPECT_LIMITED_STACK(code, 20 + heap_overhead + ptr_size, expected);
		EXPECT_EQ(Heap_Ptr::end, Stack_Ptr::begin);
	}

	TEST(heap_tests, simple_alloc_and_free) {
		signed char code[] { PUSH_BYTE(10), op_new, op_free };
		signed char expected[] { };
		EXPECT_LIMITED_STACK(code, 10 + heap_overhead + ptr_size, expected);
		EXPECT_EQ(Heap_Ptr::end, Ram_Ptr::begin);
	}

	TEST(heap_tests, free_list) {
		int block_size { 2 * Int::typed_size };
		signed char code[] {
			PUSH_BYTE(block_size), op_new, PUSH_BYTE(block_size), op_new,
				op_dup, PUSH_SMALL_INT(21), op_swap, PUSH_BYTE(0), op_swap, op_send,
				op_dup, PUSH_SMALL_INT(22), op_swap,
					PUSH_BYTE(Int::typed_size), op_swap, op_send,
			op_swap, op_free, PUSH_BYTE(block_size), op_new,
				op_dup, PUSH_SMALL_INT(31), op_swap, PUSH_BYTE(0), op_swap, op_send,
				op_dup, PUSH_SMALL_INT(32), op_swap,
					PUSH_BYTE(Int::typed_size), op_swap, op_send,
		};
		int heap_size { 2 * (block_size + heap_overhead) };
		signed char expected[] {
			RAW_PTR(heap_overhead), RAW_PTR(2 * heap_overhead + block_size)
		};
		auto stack_size { sizeof(expected) };
		//Enable_Dump enable_dump { true, true, true };
		EXPECT_LIMITED_STACK(
			code, heap_size + stack_size + ptr_size + Int::typed_size + Byte::typed_size,
			expected
		);
		EXPECT_EQ(Heap_Ptr::end + 2 * Int::typed_size + Byte::typed_size, Stack_Ptr::begin);
	}


	TEST(heap_tests, fragmentation) {
		signed char code[] {
			PUSH_BYTE(Byte::typed_size), op_new,
				op_dup, PUSH_BYTE(10), op_swap, PUSH_BYTE(0), op_swap, op_send,
			PUSH_BYTE(Byte::typed_size), op_new,
				op_dup, PUSH_BYTE(20), op_swap, PUSH_BYTE(0), op_swap, op_send,
			op_swap, op_free, PUSH_BYTE(2 * Int::typed_size), op_new
		};
		// Enable_Dump enable_dump { true, true, true };
		EXPECT_LIMITED_STACK_ERROR(
			code,
			Byte::typed_size + heap_overhead + Int::typed_size + heap_overhead +
				3 * ptr_size + 2 * Byte::typed_size,
			Err::heap_overflow
		);
	}
#endif