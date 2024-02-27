#pragma once

#include "ptr.h"
#include "value.h"
#include "vm.h"

namespace vm {
	class Acc {
		public:
			template<typename P> static int get_int(const P& ptr);

			template<typename P> static void set_int(P ptr, int value);

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
			static signed char pull_ch();
			static int pull_int();

			#if CONFIG_WITH_HEAP
				static Heap_Ptr pull_ptr();
			#endif

			static Stack_Ptr push(Value value, int after_values = 0);
	};
}