#include "../accessor.h"
#include "xor.h"

using namespace vm;

void vm::ops::Xor::perform_ch(signed char a, signed char b) {
	Acc::push(to_ch(a ^ b, Err::unexpected, Err::unexpected));
}

void vm::ops::Xor::perform_int(int a, int b) { Acc::push(a ^ b); }
