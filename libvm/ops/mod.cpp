#include "mod.h"

using namespace vm;

#if CONFIG_WITH_INT
	Value vm::ops::Mod::perform_int(const Int& a, const Int& b) const {
		if (b.value == 0) { err(Err::mod_divide_by_0); }
		#if CONFIG_OBERON_MATH
			int value { a.value % b.value };
			if (value < 0) {
				if (b.value > 0) { value += b.value; } else { value -= b.value; }
			}
			return value);
		#else
			return Int { a.value % b.value };
		#endif
	}
#endif
