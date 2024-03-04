#include "../accessor.h"
#include "mod.h"

using namespace vm;

#if CONFIG_WITH_INT
	void vm::ops::Mod::perform_int(const Int& a, const Int& b) {
		if (b.value == 0) { err(Err::mod_divide_by_0); }
		#if CONFIG_OBERON_MATH
			int value { a.value % b.value };
			if (value < 0) {
				if (b.value > 0) { value += b.value; } else { value -= b.value; }
			}
			Acc::push(value);
		#else
			Acc::push(Int { a.value % b.value });
		#endif
	}
#endif
