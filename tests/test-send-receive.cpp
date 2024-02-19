#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(send_receive_tests, simple) {
	signed char code[] {
		PUSH_CH(10), op_new, PUSH_CH(44), PUSH_CH(heap_overhead), op_send, PUSH_CH(heap_overhead), op_receive
	};
	signed char expected[] { RAW_CH(44), RAW_PTR(heap_overhead) };
	EXPECT_LIMITED_STACK(code, 10 + heap_overhead + ptr_size + 2 * ch_size, expected);
	EXPECT_EQ(heap_end() + ch_size, stack_begin());
}
