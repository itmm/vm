#include <limits>

#include "../accessor.h"
#include "add.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	void vm::ops::Add::perform_byte(const Byte& a, const Byte& b) {
		Acc::push(Byte { to_ch(
			a.value + b.value, Err::add_overflow, Err::add_underflow
		) } );
	}
#endif

#if CONFIG_WITH_INT
	void vm::ops::Add::perform_int(const Int& a, const Int& b) {
		if (a.value > 0 && b.value > 0 && std::numeric_limits<Int::value_type>::max() - a.value < b.value) {
			err(Err::add_overflow);
		}
		if (a.value < 0 && b.value < 0 && std::numeric_limits<Int::value_type>::min() - a.value > b.value) {
			err(Err::add_underflow);
		}
		Acc::push(Int { a.value + b.value });
	}
#endif
