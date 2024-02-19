#include "../accessor.h"
#include "or.h"

using namespace vm;

void vm::ops::Or::perform_ch(signed char a, signed char b) {
	Accessor::push(to_ch(a | b, Err::unexpected, Err::unexpected));
}

void vm::ops::Or::perform_int(int a, int b) { Accessor::push(a | b); }
