#include <limits>

#include "../accessor.h"
#include "sub.h"

using namespace vm;

#if CONFIG_WITH_CHAR
	void vm::ops::Sub::perform_ch(signed char a, signed char b) {
		Acc::push(Char { to_ch(
			a - b, Err::sub_overflow, Err::sub_underflow
		) } );
	}
#endif

#if CONFIG_WITH_INT
	void vm::ops::Sub::perform_int(int a, int b) {
		if (a > 0 && b < 0 && a > std::numeric_limits<int>::max() + b) {
			err(Err::sub_overflow);
		}
		if (a < 0 && b > 0 && a < std::numeric_limits<int>::min() + b) {
			err(Err::sub_underflow);
		}
		Acc::push(Int { a - b });
	}
#endif
