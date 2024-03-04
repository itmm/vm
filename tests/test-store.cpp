#include "gtest/gtest.h"

#include "vm-tests.h"

using namespace vm;

#if CONFIG_WITH_INT
	#if CONFIG_WITH_BYTE
		TEST(store_tests, small_store_ch) {
			signed char code[] {
				PUSH_BYTE(10), PUSH_BYTE(11), PUSH_BYTE(20), PUSH_BYTE(Byte::typed_size), op_store
			};
			signed char expected[] { RAW_BYTE(11), RAW_BYTE(20) };
			EXPECT_LIMITED_STACK(code, 4 * Byte::typed_size, expected);
		}

		TEST(store_tests, store_ch) {
			signed char code[] {
				PUSH_BYTE(10), PUSH_BYTE(11), PUSH_BYTE(20), PUSH_SMALL_INT(Byte::typed_size), op_store
			};
			signed char expected[] { RAW_BYTE(11), RAW_BYTE(20) };
			EXPECT_LIMITED_STACK(code, 3 * Byte::typed_size + Int::typed_size, expected);
		}
	#endif

	TEST(store_tests, store_int) {
		signed char code[] {
			PUSH_SMALL_INT(10), PUSH_SMALL_INT(11), PUSH_SMALL_INT(20),
			PUSH_SMALL_INT(Int::typed_size), op_store
		};
		signed char expected[] { RAW_INT(11), RAW_INT(20) };
		EXPECT_LIMITED_STACK(code, 4 * Int::typed_size, expected);
	}
#endif