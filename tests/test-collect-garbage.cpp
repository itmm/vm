#include "gtest/gtest.h"

#include "ptr.h"
#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_GC
	TEST(collect_garbage_tests, simple) {
		int block_size { 10 };
		int raw_block_size { block_size + heap_overhead };

		signed char code[] {
			PUSH_BYTE(block_size), op_new, PUSH_BYTE(block_size), op_new,
			op_swap, op_free, op_pull, op_collect_garbage
		};
		signed char expected[] { };
		//Enable_Dump enable_dump { true, true, true };
		EXPECT_LIMITED_STACK(code, 2 * raw_block_size + 2 * ptr_size, expected);
		EXPECT_EQ(Heap_Ptr::end, Ram_Ptr::begin);
	}

	TEST(collect_garbage_tests, auto_collect) {
		int block_size { 10 };
		int raw_block_size { block_size + heap_overhead };

		signed char code[] {
			PUSH_BYTE(block_size), op_new, op_pull,
			PUSH_BYTE(block_size), op_new
		};
		signed char expected[] { RAW_PTR(heap_overhead) };
		//Enable_Dump enable_dump { true, true, true };
		EXPECT_LIMITED_STACK(code, raw_block_size + ptr_size, expected);
	}
#endif
