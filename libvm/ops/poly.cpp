#include "poly.h"

using namespace vm;

#if CONFIG_WITH_NUMERIC
	Value ops::Poly::operator()(const Value& a, const Value& b) const {
		#if CONFIG_WITH_BYTE
			auto a_ch = std::get_if<Byte>(&a);
			auto b_ch = std::get_if<Byte>(&b);

			if (a_ch && b_ch) { return perform_byte(*a_ch, *b_ch); }
		#endif

		#if CONFIG_WITH_INT
			auto a_int = std::get_if<Int>(&a);
			auto b_int = std::get_if<Int>(&b);

			#if CONFIG_WITH_BYTE
				if ((a_int || a_ch) && (b_int || b_ch)) {
					return perform_int(
						a_int ? *a_int : Int { a_ch->value },
						b_int ? *b_int : Int { b_ch->value }
					);
				}
			#else
				if (a_int && b_int) { return perform_int(a_int, b_int); }
			#endif
		#endif
		err(Err::unknown_type);
	}
#endif

#if CONFIG_WITH_BYTE
	Value ops::Poly::perform_byte(const Byte& a, const Byte& b) const {
		return perform_int(Int { a.value }, Int { b.value });
	}
#endif

#if CONFIG_WITH_INT
	Value ops::Poly::perform_int(const Int& a, const Int& b) const {
		err(vm::Err::unknown_type);
	}
#endif