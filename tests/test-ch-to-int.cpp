#include "gtest/gtest.h"

#include "vm-tests.h"
#include "vm.h"

TEST(ch_to_int_tests, zero) {
	signed char code[] {
		vm::op_push_ch, 0, vm::op_ch_to_int, vm::op_break
	};
	signed char expected[] { 0, 0, 0, 0 };
	EXPECT_STACK(code, expected);
}

TEST(ch_to_int_tests, simple) {
	signed char code[] {
		vm::op_push_ch, 10, vm::op_ch_to_int, vm::op_break
	};
	signed char expected[] { 0, 0, 0, 10 };
	EXPECT_STACK(code, expected);
}

TEST(ch_to_int_tests, negative) {
	signed char code[] {
		vm::op_push_ch, -10, vm::op_ch_to_int, vm::op_break
	};
	signed char expected[] { -1, -1, -1, -10 };
	EXPECT_STACK(code, expected);
}
