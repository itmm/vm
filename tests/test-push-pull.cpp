#include "gtest/gtest.h"

#include "vm-tests.h"
#include "vm.h"

TEST(push_pull_tests, empty) {
	signed char code[] { vm::op_break };
	signed char expected[] {};
	EXPECT_STACK(code, expected);
}

TEST(push_pull_tests, push_3) {
	signed char code[] {
		vm::op_push_ch, 10, vm::op_push_ch, 20,
		vm::op_push_ch, 30, vm::op_break
	};
	signed char expected[] { 30, 20, 10 };
	EXPECT_STACK(code, expected);
}