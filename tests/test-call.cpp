#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_CALL
	TEST(call_tests, simple) {
		signed char code[] {
			op_near_jmp, 1,
			op_return,
			PUSH_BYTE(0), PUSH_BYTE(2), op_call,
			PUSH_BYTE(0), PUSH_BYTE(2), op_call
		};
		signed char expected[] { };
		EXPECT_LIMITED_STACK(
			code, Stack_Frame::typed_size + Byte::typed_size, expected
		);
	}

	TEST(call_tests, return_with_noempty_stack) {
		signed char code[] {
			op_near_jmp, 3,
			PUSH_BYTE(10), op_return,
			PUSH_BYTE(0), PUSH_BYTE(2), op_call,
			PUSH_BYTE(0), PUSH_BYTE(2), op_call
		};
		signed char expected[] { };
		EXPECT_LIMITED_STACK(
			code, Stack_Frame::typed_size + 2 * Byte::typed_size, expected
		);
	}

	TEST(call_tests, return_with_value) {
		signed char code[] {
			op_near_jmp, 3,
			PUSH_BYTE(10), op_return_value,
			PUSH_BYTE(0), PUSH_BYTE(2), op_call,
			PUSH_BYTE(0), PUSH_BYTE(2), op_call
		};
		signed char expected[] { RAW_BYTE(10), RAW_BYTE(10) };
		EXPECT_LIMITED_STACK(
			code, Stack_Frame::typed_size + 4 * Byte::typed_size, expected
		);
	}
#endif
