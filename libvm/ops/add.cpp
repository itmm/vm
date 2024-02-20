#include <limits>

#include "../accessor.h"
#include "add.h"

using namespace vm;

void vm::ops::Add::perform_ch(signed char a, signed char b) {
	Accessor::push(to_ch(
		a + b, Err::add_overflow, Err::add_underflow
	));
}

void vm::ops::Add::perform_int(int a, int b) {
	if (a > 0 && b > 0 && std::numeric_limits<int>::max() - a < b) {
		err(Err::add_overflow);
	}
	if (a < 0 && b < 0 && std::numeric_limits<int>::min() - a > b) {
		err(Err::add_underflow);
	}
	Accessor::push(a + b);
}
