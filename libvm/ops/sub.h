#pragma once

#include "poly.h"

namespace vm::ops {
	class Sub : public ops::Poly {
	public:
		Sub() = default;

		void perform_ch(signed char a, signed char b) override;

		void perform_int(int a, int b) override;
	};

}
