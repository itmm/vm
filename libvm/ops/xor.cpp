#include "../accessor.h"
#include "xor.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	void vm::ops::Xor::perform_ch(signed char a, signed char b) {
		Acc::push(Byte { to_ch(a ^ b, Err::unexpected, Err::unexpected) });
	}
#endif

#if CONFIG_WITH_INT
	void vm::ops::Xor::perform_int(int a, int b) { Acc::push(Int { a ^ b }); }
#endif
