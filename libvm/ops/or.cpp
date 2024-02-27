#include "../accessor.h"
#include "or.h"

using namespace vm;

#if CONFIG_WITH_CHAR
	void vm::ops::Or::perform_ch(signed char a, signed char b) {
		Acc::push(to_ch(a | b, Err::unexpected, Err::unexpected));
	}
#endif

#if CONFIG_WITH_INT
	void vm::ops::Or::perform_int(int a, int b) { Acc::push(a | b); }
#endif
