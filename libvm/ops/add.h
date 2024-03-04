#pragma once

#include "poly.h"

#if CONFIG_WITH_NUMERIC
	namespace vm::ops {
		class Add : public ops::Poly {
		public:
			Add() = default;

			#if CONFIG_WITH_BYTE
				void perform_byte(const Byte& a, const Byte& b) override;
			#endif
			#if CONFIG_WITH_INT
				void perform_int(int a, int b) override;
			#endif
		};
	}
#endif