#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

TEST(call_tests, simple) {
	signed char code[] {
		op_near_jmp, 1,
		op_return,
		PUSH_CH(2), op_call,
		PUSH_CH(2), op_call
	};
	signed char expected[] { };
	EXPECT_LIMITED_STACK(code, stack_frame_size, expected);
}
