#pragma once

#include "poly.h"

#if CONFIG_WITH_INT
	namespace vm::ops {
		class Mod : public ops::Poly {
		public:
			Mod() = default;

			void perform_int(const Int& a, const Int& b) override;
		};
	}
#endif
