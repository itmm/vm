#pragma once

namespace vm::ops {
	class Poly {
		public:
			void operator()();
			virtual ~Poly() = default;

		protected:
			Poly() = default;
			virtual void perform_ch(signed char a, signed char b);
			virtual void perform_int(int a, int b);
	};
}
