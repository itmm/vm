#pragma once

#include "poly.h"

namespace vm::ops {
	class Mod : public ops::Poly {
	public:
		Mod() = default;

		void perform_ch(signed char a, signed char b) override;

		void perform_int(int a, int b) override;
	};

}
