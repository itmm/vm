#include "../accessor.h"
#include "poly.h"

using namespace vm;

void ops::Poly::operator()() {
	auto b { Acc::pull() }; auto a { Acc::pull() };
	auto a_ch = std::get_if<signed char>(&a);
	auto b_ch = std::get_if<signed char>(&b);

	if (a_ch && b_ch) { perform_ch(*a_ch, *b_ch); return; }

	auto a_int = std::get_if<int>(&a);
	auto b_int = std::get_if<int>(&b);

	if ((a_int || a_ch) && (b_int || b_ch)) {
		perform_int(a_int ? *a_int : *a_ch, b_int ? *b_int : *b_ch);
		return;
	}
	err(Err::unknown_type);
}

void ops::Poly::perform_ch(signed char a, signed char b) {
	err(vm::Err::unknown_type);
}
void ops::Poly::perform_int(int a, int b) {
	err(vm::Err::unknown_type);
}
