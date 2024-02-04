#include "gtest/gtest.h"

#include "vm-tests.h"
#include "vm.h"

TEST(add_tests, simple) {
	signed char code[] {
		vm::op_push_ch, 10, vm::op_ch_to_int,
		vm::op_push_ch, 20, vm::op_ch_to_int,
		vm::op_add_int, vm::op_break
	};
	signed char expected[] { 0, 0, 0, 30 };
	EXPECT_STACK(code, expected);
}

TEST(add_tests, negative) {
	signed char code[] {
		vm::op_push_ch, -10, vm::op_ch_to_int,
		vm::op_push_ch, 0, vm::op_ch_to_int,
		vm::op_add_int, vm::op_break
	};
	signed char expected[] { -1, -1, -1, -10 };
	EXPECT_STACK(code, expected);
}

TEST(add_tests, big_ok_a) {
	signed char code[] {
		vm::op_push_ch, -1, vm::op_push_ch, -1,
		vm::op_push_ch, -1, vm::op_push_ch, 127,
		vm::op_push_ch, 0, vm::op_ch_to_int,
		vm::op_add_int, vm::op_break
	};
	signed char expected[] { 127, -1, -1, -1 };
	EXPECT_STACK(code, expected);
}

TEST(add_tests, big_ok_b) {
	signed char code[] {
		vm::op_push_ch, 0, vm::op_ch_to_int,
		vm::op_push_ch, -1, vm::op_push_ch, -1,
		vm::op_push_ch, -1, vm::op_push_ch, 127,
		vm::op_add_int, vm::op_break
	};
	signed char expected[] { 127, -1, -1, -1 };
	EXPECT_STACK(code, expected);
}