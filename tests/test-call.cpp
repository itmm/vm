#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_CALL
	TEST(call_tests, simple) {
		signed char code[] {
			op_near_jmp, 1,
			op_return,
			PUSH_CH(0), PUSH_CH(2), op_call,
			PUSH_CH(0), PUSH_CH(2), op_call
		};
		signed char expected[] { };
		EXPECT_LIMITED_STACK(code, stack_frame_size + Char::typed_size, expected);
	}

	TEST(call_tests, return_with_noempty_stack) {
		signed char code[] {
			op_near_jmp, 3,
			PUSH_CH(10), op_return,
			PUSH_CH(0), PUSH_CH(2), op_call,
			PUSH_CH(0), PUSH_CH(2), op_call
		};
		signed char expected[] { };
		EXPECT_LIMITED_STACK(
			code, stack_frame_size + 2 * Char::typed_size, expected
		);
	}
#endif
