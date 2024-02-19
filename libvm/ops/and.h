#pragma once

#include "poly.h"

namespace vm::ops {
	class And : public ops::Poly {
	public:
		And() = default;

		void perform_ch(signed char a, signed char b) override;

		void perform_int(int a, int b) override;
	};

}
