#pragma once

#if CONFIG_WITH_NUMERIC
	namespace vm::ops {
		class Poly {
			public:
				void operator()();
				virtual ~Poly() = default;

			protected:
				Poly() = default;

				#if CONFIG_WITH_BYTE
				// TODO: replace with Byte
					virtual void perform_ch(signed char a, signed char b);
				#endif
				#if CONFIG_WITH_INT
					// TODO: replace with Int
					virtual void perform_int(int a, int b);
				#endif
				// TODO: multiprecision operators
		};
	}
#endif