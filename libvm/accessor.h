#pragma once

#include "ptr.h"
#include "value.h"
#include "vm.h"

namespace vm {
	class Acc {
		public:
			#if CONFIG_WITH_INT
				template<typename P> static Int get_int(const P& ptr);
				template<typename P> static void set_int(P ptr, const Int& value);
			#endif

			template<typename P>
			static signed char get_byte(const P& ptr);

			template<typename P> static Value get_value(const P& ptr);

			template<typename P> static void set_value(P ptr, const Value& value);

			#if CONFIG_WITH_HEAP
				template<typename P> static Heap_Ptr get_ptr(const P& ptr);

				template<typename P> static void set_ptr(
					P ptr, const Heap_Ptr& value
				);
			#endif

			static Value pull();

			#if CONFIG_WITH_CHAR
				static Char pull_ch();
			#endif
			#if CONFIG_WITH_INT
				static Int pull_int();
			#endif

			#if CONFIG_WITH_HEAP
				static Heap_Ptr pull_ptr();
			#endif

			static Stack_Ptr push(Value value, int after_values = 0);
	};
}