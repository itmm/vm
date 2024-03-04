#include "../accessor.h"
#include "poly.h"

using namespace vm;

#if CONFIG_WITH_NUMERIC
	void ops::Poly::operator()() {
		auto b { Acc::pull() }; auto a { Acc::pull() };

		#if CONFIG_WITH_BYTE
			auto a_ch = std::get_if<Byte>(&a);
			auto b_ch = std::get_if<Byte>(&b);

			if (a_ch && b_ch) { perform_byte(*a_ch, *b_ch); return; }
		#endif

		#if CONFIG_WITH_INT
			auto a_int = std::get_if<Int>(&a);
			auto b_int = std::get_if<Int>(&b);

			#if CONFIG_WITH_BYTE
				if ((a_int || a_ch) && (b_int || b_ch)) {
					perform_int(
						a_int ? *a_int : Int { a_ch->value },
						b_int ? *b_int : Int { b_ch->value }
					);
					return;
				}
			#else
				if (a_int && b_int) { perform_int(a_int, b_int); return; }
			#endif
		#endif
		err(Err::unknown_type);
	}
#endif

#if CONFIG_WITH_BYTE
	void ops::Poly::perform_byte(const Byte& a, const Byte& b) {
		perform_int(Int { a.value }, Int { b.value });
	}
#endif

#if CONFIG_WITH_INT
	void ops::Poly::perform_int(const Int& a, const Int& b) {
		err(vm::Err::unknown_type);
	}
#endif