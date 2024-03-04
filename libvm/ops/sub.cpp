#include <limits>

#include "../accessor.h"
#include "sub.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	void vm::ops::Sub::perform_byte(const Byte& a, const Byte& b) {
		Acc::push(Byte { to_ch(
			a.value - b.value, Err::sub_overflow, Err::sub_underflow
		) } );
	}
#endif

#if CONFIG_WITH_INT
	void vm::ops::Sub::perform_int(const Int& a, const Int& b) {
		if (a.value > 0 && b.value < 0 && a.value > std::numeric_limits<Int::value_type>::max() + b.value) {
			err(Err::sub_overflow);
		}
		if (a.value < 0 && b.value > 0 && a.value < std::numeric_limits<Int::value_type>::min() + b.value) {
			err(Err::sub_underflow);
		}
		Acc::push(Int { a.value - b.value });
	}
#endif
