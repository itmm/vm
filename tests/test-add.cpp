#include "gtest/gtest.h"

#include "ops/add.h"
#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_INT
	TEST(add_tests, simple) {
		signed char code[] { PUSH_SMALL_INT(10), PUSH_SMALL_INT(20), op_add };
		signed char expected[] { RAW_INT(30) };
		EXPECT_LIMITED_STACK(code, 2 * Int::typed_size, expected);
	}

	TEST(add_tests, negative) {
		EXPECT_EQ(ops::Add { } (Int { 0 }, Int { -10 }), Value { Int { -10 } });
	}

	TEST(add_tests, big_ok_a) {
		EXPECT_EQ(
			ops::Add { } (Int { 0x7ffffffd }, Int { 2 }),
			Value { Int { 0x7fffffff } }
		);
	}

	TEST(add_tests, big_overflow_a) {
		signed char code[] { PUSH_INT(0x7ffffffd), PUSH_SMALL_INT(3), op_add };
		EXPECT_ERROR(code, Err::add_overflow);
	}

	TEST(add_tests, big_ok_b) {
		EXPECT_EQ(
			ops::Add { } (Int { 2 }, Int { 0x7ffffffd }),
			Value { Int { 0x7fffffff }}
		);
	}

	TEST(add_tests, big_overflow_b) {
		signed char code[] { PUSH_SMALL_INT(3), PUSH_INT(0x7ffffffd), op_add };
		EXPECT_ERROR(code, Err::add_overflow);
	}

	TEST(add_tests, two_bigs) {
		EXPECT_EQ(
			ops::Add { } (
				Int { 0x7fffffff },
				Int { static_cast<int>(0x80000000) }
			),
			Value { Int { -1 } }
		);
	}

	TEST(add_tests, no_ram) {
		signed char code[] { PUSH_SMALL_INT(5), PUSH_SMALL_INT(6), op_add };
		EXPECT_STACK_OVERFLOW(code, 7);
	}

	TEST(add_tests, underflow) {
		signed char code[] { PUSH_SMALL_INT(5), op_add };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif
