#pragma once

#include "poly.h"

#if CONFIG_WITH_INT

	namespace vm::ops {
		class Mult : public ops::Poly {
		public:
			Mult() = default;

			Value perform_int(const Int& a, const Int& b) const override;
		};
	}

#endif