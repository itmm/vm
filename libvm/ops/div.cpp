#include "../accessor.h"
#include "div.h"

using namespace vm;

#if CONFIG_WITH_INT
	Value vm::ops::Div::perform_int(const Int& a, const Int& b) const {
		if (b.value == 0) { err(Err::div_divide_by_0); }
		if (a.value == 0x80000000 && b.value == -1) { err(Err::div_overflow); }
		#if CONFIG_OBERON_MATH
			int value { a.value / b.value };
			int rem { a.value % b.value };
			if (rem < 0) { value += b.value > 0 ? -1 : 1; }
			return Int { value };
		#else
			return Int { a.value / b.value };
		#endif
	}
#endif