#pragma once

#include "../value.h"

#if CONFIG_WITH_NUMERIC
	namespace vm::ops {
		class Poly {
			public:
				Value operator()(const Value& a, const Value&b) const;
				virtual ~Poly() = default;

				#if CONFIG_WITH_BYTE
					virtual Value perform_byte(const Byte& a, const Byte& b) const;
				#endif
				#if CONFIG_WITH_INT
					virtual Value perform_int(const Int& a, const Int& b) const;
				#endif

			protected:
				Poly() = default;
		};
	}
#endif