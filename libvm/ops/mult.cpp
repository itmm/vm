#include "../accessor.h"
#include "mult.h"

using namespace vm;

#if CONFIG_WITH_INT
	Value vm::ops::Mult::perform_int(const Int& a, const Int& b) const {
		if (a.value == 0x80000000 && b.value == -1) { err(Err::mult_overflow); }
		int value { a.value * b.value };
		if (b.value != 0 && value / b.value != a.value) {
			if ((a.value < 0 && b.value > 0) || (a.value > 0 && b.value < 0)) {
				err(Err::mult_underflow);
			}
			err(Err::mult_overflow);
		}
		return Int { value };
	}
#endif