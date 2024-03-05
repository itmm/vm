#include <limits>

#include "sub.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	Value vm::ops::Sub::perform_byte(const Byte& a, const Byte& b) const {
		return Byte { to_byte(
			a.value - b.value, Err::sub_overflow, Err::sub_underflow
		) };
	}
#endif

#if CONFIG_WITH_INT
	Value vm::ops::Sub::perform_int(const Int& a, const Int& b) const {
		if (a.value > 0 && b.value < 0 && a.value > std::numeric_limits<Int::value_type>::max() + b.value) {
			err(Err::sub_overflow);
		}
		if (a.value < 0 && b.value > 0 && a.value < std::numeric_limits<Int::value_type>::min() + b.value) {
			err(Err::sub_underflow);
		}
		return Int { a.value - b.value };
	}
#endif
