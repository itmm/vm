#include "../accessor.h"
#include "and.h"

using namespace vm;

#if CONFIG_WITH_CHAR
	void vm::ops::And::perform_ch(signed char a, signed char b) {
		Acc::push(Char { to_ch(a & b, Err::unexpected, Err::unexpected) });
	}
#endif

#if CONFIG_WITH_INT
	void vm::ops::And::perform_int(int a, int b) { Acc::push(Int { a & b }); }
#endif
