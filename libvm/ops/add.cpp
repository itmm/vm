#include <limits>

#include "../accessor.h"
#include "add.h"

using namespace vm;

#if CONFIG_WITH_BYTE
	void vm::ops::Add::perform_byte(const Byte& a, const Byte& b) {
		Acc::push(Byte { to_ch(
			a.value + b.value, Err::add_overflow, Err::add_underflow
		) } );
	}
#endif

#if CONFIG_WITH_INT
	void vm::ops::Add::perform_int(int a, int b) {
		if (a > 0 && b > 0 && std::numeric_limits<int>::max() - a < b) {
			err(Err::add_overflow);
		}
		if (a < 0 && b < 0 && std::numeric_limits<int>::min() - a > b) {
			err(Err::add_underflow);
		}
		Acc::push(Int { a + b });
	}
#endif
