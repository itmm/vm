#include "gtest/gtest.h"

#include "vm-tests.h"
#include "ptr.h"

using namespace vm;

TEST(send_receive_tests, simple) {
	auto block_size { 2 * int_size };
	signed char code[] {
		PUSH_CH(block_size), op_new,
			op_dup, PUSH_SMALL_INT(44), op_swap, PUSH_CH(0), op_swap, op_send,
			op_dup, PUSH_SMALL_INT(55), op_swap, PUSH_CH(int_size), op_swap, op_send,
			op_dup, PUSH_CH(0), op_swap, op_receive
	};
	signed char expected[] { RAW_INT(44), RAW_PTR(heap_overhead) };
	Enable_Dump enable_dump { true, true, true };
	EXPECT_LIMITED_STACK(code, block_size + heap_overhead + 2 * ptr_size + int_size + ch_size, expected);
	EXPECT_EQ(heap_end + int_size + ch_size, stack_begin);
}
