#include "../accessor.h"
#include "div.h"

using namespace vm;

#if CONFIG_WITH_INT
	void vm::ops::Div::perform_int(int a, int b) {
		if (b == 0) { err(Err::div_divide_by_0); }
		if (a == 0x80000000 && b == -1) { err(Err::div_overflow); }
		#if CONFIG_OBERON_MATH
			int value { a / b };
			int rem { a % b };
			if (rem < 0) { value += b > 0 ? -1 : 1; }
			Acc::push(Int { value });
		#else
			Acc::push(Int { a / b });
		#endif
	}
#endif