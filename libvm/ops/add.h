#pragma once

#include "poly.h"

#if CONFIG_WITH_NUMERIC
	namespace vm::ops {
		class Add : public ops::Poly {
		public:
			Add() = default;

			void perform_ch(signed char a, signed char b) override;

			void perform_int(int a, int b) override;
		};
	}
#endif