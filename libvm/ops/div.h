#pragma once

#include "poly.h"

#if CONFIG_WITH_INT
	namespace vm::ops {
		class Div : public ops::Poly {
		public:
			Div() = default;

			Value perform_int(const Int& a, const Int& b) const override;
		};
	}
#endif
