#pragma once

#include "poly.h"

#if CONFIG_WITH_INT
	namespace vm::ops {
		class Mod : public ops::Poly {
		public:
			Mod() = default;

			void perform_int(int a, int b) override;
		};
	}
#endif
