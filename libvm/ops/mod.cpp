#include "../accessor.h"
#include "mod.h"

using namespace vm;

void vm::ops::Mod::perform_ch(signed char a, signed char b) {
	if (b == 0) { err(Err::mod_divide_by_0); }
	#if CONFIG_OBERON_MATH
		int value = a % b;
		if (value < 0) {
			if (b > 0) { value += b; } else { value -= b; }
		}
		Acc::push(to_ch(value, Err::unexpected, Err::unexpected));
	#else
		Acc::push_value(to_ch(a % b, Err::unexpected, Err::unexpected));
	#endif
}

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
