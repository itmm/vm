#pragma once

#include "poly.h"

#if CONFIG_WITH_NUMERIC
	namespace vm::ops {
		class Or : public ops::Poly {
		public:
			Or() = default;

			#if CONFIG_WITH_BYTE
				Value perform_byte(const Byte& a, const Byte& b) const override;
			#endif
			#if CONFIG_WITH_INT
				Value perform_int(const Int& a, const Int& b) const override;
			#endif
		};
	}
#endif
