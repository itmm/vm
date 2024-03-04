#include "gtest/gtest.h"

#include "vm-tests.h"
#include "ptr.h"

using namespace vm;

#if CONFIG_WITH_HEAP
	TEST(send_receive_tests, simple) {
		auto block_size { 2 * Int::typed_size };
		signed char code[] {
			PUSH_BYTE(block_size), op_new,
				op_dup, PUSH_SMALL_INT(44), op_swap, PUSH_BYTE(0), op_swap, op_send,
				op_dup, PUSH_SMALL_INT(55), op_swap, PUSH_BYTE(Int::typed_size), op_swap, op_send,
				op_dup, PUSH_BYTE(0), op_swap, op_receive
		};
		signed char expected[] { RAW_INT(44), RAW_PTR(heap_overhead) };
		//Enable_Dump enable_dump { true, true, true };
		EXPECT_LIMITED_STACK(code, block_size + heap_overhead + 2 * ptr_size + Int::typed_size + Byte::typed_size, expected);
		EXPECT_EQ(Heap_Ptr::end + Int::typed_size + Byte::typed_size, Stack_Ptr::begin);
	}
#endif
