#include "../accessor.h"
#include "poly.h"

using namespace vm;

#if CONFIG_WITH_NUMERIC

void ops::Poly::operator()() {
	auto b { Acc::pull() }; auto a { Acc::pull() };

	#if CONFIG_WITH_CHAR
		auto a_ch = std::get_if<Char>(&a);
		auto b_ch = std::get_if<Char>(&b);

		if (a_ch && b_ch) { perform_ch(a_ch->value, b_ch->value); return; }
	#endif

	#if CONFIG_WITH_INT
		auto a_int = std::get_if<Int>(&a);
		auto b_int = std::get_if<Int>(&b);

		#if CONFIG_WITH_CHAR
			if ((a_int || a_ch) && (b_int || b_ch)) {
				perform_int(
					a_int ? a_int->value : a_ch->value,
					b_int ? b_int->value : b_ch->value
				);
				return;
			}
		#else
			if (a_int && b_int) { perform_int(*a_int, *b_int); return; }
		#endif
	#endif
	err(Err::unknown_type);
}
#endif

#if CONFIG_WITH_CHAR
	void ops::Poly::perform_ch(signed char a, signed char b) {
		perform_int(a, b);
	}
#endif

#if CONFIG_WITH_INT
	void ops::Poly::perform_int(int a, int b) {
		err(vm::Err::unknown_type);
	}
#endif