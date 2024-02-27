#pragma once

#include "poly.h"

#if CONFIG_WITH_NUMERIC
	namespace vm::ops {
		class Or : public ops::Poly {
		public:
			Or() = default;

			#if CONFIG_WITH_CHAR
				void perform_ch(signed char a, signed char b) override;
			#endif
			#if CONFIG_WITH_INT
				void perform_int(int a, int b) override;
			#endif
		};
	}
#endif
