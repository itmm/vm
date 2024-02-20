#include "../accessor.h"
#include "mult.h"

using namespace vm;

void vm::ops::Mult::perform_ch(signed char a, signed char b) {
	Accessor::push(to_ch(a * b, Err::mult_overflow, Err::mult_underflow));
}

void vm::ops::Mult::perform_int(int a, int b) {
	if (a == 0x80000000 && b == -1) { err(Err::mult_overflow); }
	int value { a * b };
	if (b != 0 && value / b != a) {
		if ((a < 0 && b > 0) || (a > 0 && b < 0)) {
			err(Err::mult_underflow);
		}
		err(Err::mult_overflow);
	}
	Accessor::push(value);
}
