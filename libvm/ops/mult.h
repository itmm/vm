#pragma once

#include "poly.h"

#if CONFIG_WITH_INT

	namespace vm::ops {
		class Mult : public ops::Poly {
		public:
			Mult() = default;

			void perform_int(int a, int b) override;
		};
	}

#endif