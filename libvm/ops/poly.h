#pragma once

#include "../defines.h"

#if CONFIG_WITH_NUMERIC
	namespace vm::ops {
		class Poly {
			public:
				void operator()();
				virtual ~Poly() = default;

			protected:
				Poly() = default;

				#if CONFIG_WITH_BYTE
					virtual void perform_byte(const Byte& a, const Byte& b);
				#endif
				#if CONFIG_WITH_INT
					// TODO: replace with Int
					virtual void perform_int(int a, int b);
				#endif
				// TODO: multiprecision operators
		};
	}
#endif