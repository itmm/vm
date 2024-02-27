#pragma once

#include "poly.h"

#if CONFIG_WITH_INT
	namespace vm::ops {
		class Div : public ops::Poly {
		public:
			Div() = default;

			void perform_int(int a, int b) override;
		};
	}
#endif
