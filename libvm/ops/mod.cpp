#include "../accessor.h"
#include "mod.h"

using namespace vm;

#if CONFIG_WITH_INT
	void vm::ops::Mod::perform_int(int a, int b) {
		if (b == 0) { err(Err::mod_divide_by_0); }
		#if CONFIG_OBERON_MATH
			int value { a % b };
			if (value < 0) {
				if (b > 0) { value += b; } else { value -= b; }
			}
			Acc::push(value);
		#else
			Acc::push(a % b);
		#endif
	}
#endif
