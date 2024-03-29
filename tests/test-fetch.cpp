#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_INT
	#if CONFIG_WITH_BYTE
		TEST(fetch_tests, fetch_ch) {
			signed char code[] {
				PUSH_BYTE(10), PUSH_BYTE(20), PUSH_SMALL_INT(Byte::typed_size), op_fetch
			};
			signed char expected[] { RAW_BYTE(10), RAW_BYTE(20), RAW_BYTE(10) };
			EXPECT_LIMITED_STACK(code, 2 * Byte::typed_size + Int::typed_size, expected);
		}

		TEST(fetch_tests, small_fetch_ch) {
			signed char code[] {
				PUSH_BYTE(10), PUSH_BYTE(20), PUSH_BYTE(Byte::typed_size), op_fetch
			};
			signed char expected[] { RAW_BYTE(10), RAW_BYTE(20), RAW_BYTE(10) };
			EXPECT_LIMITED_STACK(code, 3 * Byte::typed_size, expected);
		}

		TEST(fetch_tests, ch_out_of_range) {
			signed char code[] {
				PUSH_BYTE(10), PUSH_SMALL_INT(Byte::typed_size), op_fetch
			};
			EXPECT_ERROR(code, Err::leave_stack_segment);
		}

		TEST(fetch_tests, small_fetch_int) {
			signed char code[] {
				PUSH_SMALL_INT(10), PUSH_SMALL_INT(20),
				PUSH_BYTE(Int::typed_size), op_fetch
			};
			signed char expected[] { RAW_INT(10), RAW_INT(20), RAW_INT(10) };
			EXPECT_LIMITED_STACK(code, 3 * Int::typed_size, expected);
		}
	#endif

	TEST(fetch_tests, fetch_int) {
		signed char code[] {
			PUSH_SMALL_INT(10), PUSH_SMALL_INT(20),
			PUSH_SMALL_INT(Int::typed_size), op_fetch
		};
		signed char expected[] { RAW_INT(10), RAW_INT(20), RAW_INT(10) };
		EXPECT_LIMITED_STACK(code, 3 * Int::typed_size, expected);
	}

	TEST(fetch_tests, int_out_of_range) {
		signed char code[] { PUSH_SMALL_INT(10), PUSH_SMALL_INT(Int::typed_size), op_fetch };
		EXPECT_ERROR(code, Err::leave_stack_segment);
	}
#endif