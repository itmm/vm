#include "../accessor.h"
#include "and.h"

using namespace vm;

void vm::ops::And::perform_ch(signed char a, signed char b) {
	Acc::push(to_ch(a & b, Err::unexpected, Err::unexpected));
}

void vm::ops::And::perform_int(int a, int b) { Acc::push(a & b); }
