#include <limits>

#include "../accessor.h"
#include "add.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	Value vm::ops::Add::perform_byte(const Byte& a, const Byte& b) const {
		return Byte { to_ch(
			a.value + b.value, Err::add_overflow, Err::add_underflow
		) };
	}
#endif

#if CONFIG_WITH_INT
	Value vm::ops::Add::perform_int(const Int& a, const Int& b) const {
		if (
			a.value > 0 && b.value > 0 &&
			std::numeric_limits<Int::value_type>::max() - a.value < b.value
		) { err(Err::add_overflow); }
		if (
			a.value < 0 && b.value < 0 &&
			std::numeric_limits<Int::value_type>::min() - a.value > b.value
		) { err(Err::add_underflow); }
		return Int { a.value + b.value };
	}
#endif
