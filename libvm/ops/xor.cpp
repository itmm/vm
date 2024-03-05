#include "../accessor.h"
#include "xor.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	Value vm::ops::Xor::perform_byte(const Byte& a, const Byte& b) const {
		return Byte { to_byte(a.value ^ b.value) };
	}
#endif

#if CONFIG_WITH_INT
	Value vm::ops::Xor::perform_int(const Int& a, const Int& b) const {
		return Int { a.value ^ b.value };
	}
#endif
