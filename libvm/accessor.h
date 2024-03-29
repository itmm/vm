#pragma once

#include "ptr.h"
#include "value.h"
#include "vm.h"

namespace vm {
	class Acc {
		public:
			#if CONFIG_WITH_INT
				template<typename P> static Int get_int(const P& ptr);
				template<typename P> static void set_int(
					P ptr, const Int& value
				);
			#endif

			template<typename P> static signed char get_byte(const P& ptr);

			template<typename P> static Value get_value(const P& ptr);

			template<typename P> static void set_value(
				P ptr, const Value& value
			);

			#if CONFIG_WITH_HEAP
				template<typename P> static Heap_Ptr get_ptr(const P& ptr);

				template<typename P> static void set_ptr(
					P ptr, const Heap_Ptr& value
				);
			#endif

			static Value pull();

			#if CONFIG_WITH_BYTE
				static Byte pull_ch();
			#endif
			#if CONFIG_WITH_NUMERIC
				static Int::internal_type pull_internal_int();
			#endif
			#if CONFIG_WITH_INT
				#if CONFIG_INTERNAL_INT_IS_INT
					static Int pull_int() {
						return Int { pull_internal_int() };
					}
				#else
					static Int pull_int();
				#endif
			#endif

			#if CONFIG_WITH_HEAP
				static Heap_Ptr pull_ptr();
			#endif

			static Stack_Ptr push(Value value, int after_values = 0);
	};
}