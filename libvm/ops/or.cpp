#include "../accessor.h"
#include "or.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	void vm::ops::Or::perform_byte(const Byte& a, const Byte& b) {
		Acc::push(Byte { to_ch(a.value | b.value, Err::unexpected, Err::unexpected) });
	}
#endif

#if CONFIG_WITH_INT
	void vm::ops::Or::perform_int(int a, int b) { Acc::push(Int { a | b }); }
#endif
