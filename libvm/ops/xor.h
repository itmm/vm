#pragma once

#include "poly.h"

#if CONFIG_WITH_NUMERIC
	namespace vm::ops {
		class Xor : public ops::Poly {
		public:
			Xor() = default;

			#if CONFIG_WITH_BYTE
				void perform_byte(const Byte& a, const Byte& b) override;
			#endif
			#if CONFIG_WITH_INT
				void perform_int(const Int& a, const Int& b) override;
			#endif
		};
	}
#endif