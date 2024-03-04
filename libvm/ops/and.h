#pragma once

#include "poly.h"

#if CONFIG_WITH_NUMERIC
	namespace vm::ops {
		class And : public ops::Poly {
		public:
			And() = default;

			#if CONFIG_WITH_BYTE
				void perform_ch(signed char a, signed char b) override;
			#endif
			#if CONFIG_WITH_INT
				void perform_int(int a, int b) override;
			#endif
		};
	}
#endif
