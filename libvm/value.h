#pragma once

#include <variant>

#include "defines.h"

#include "ptr.h"

namespace vm {
	#if CONFIG_WITH_CALL
		struct Stack_Frame {
			Code_Ptr pc;
			Ram_Ptr parent;
			Ram_Ptr outer;
		};

		bool operator==(const Stack_Frame& a, const Stack_Frame& b);
		bool operator<(const Stack_Frame& a, const Stack_Frame& b);
	#endif

	using Value = std::variant<
		#if CONFIG_WITH_CHAR
			#define VARIANT_SEP 1
			signed char
		#endif
		#if CONFIG_WITH_INT
			#if !defined(VARIANT_SEP)
				#define VARIANT_SEP 1
			#else
				,
			#endif
			int
		#endif
		#if CONFIG_WITH_HEAP
			#if !defined(VARIANT_SEP)
				#define VARIANT_SEP 1
			#else
				,
			#endif
			Heap_Ptr
		#endif
		#if CONFIG_WITH_CALL
			#if !defined(VARIANT_SEP)
				#define VARIANT_SEP 1
			#else
				,
			#endif
			Stack_Frame
		#endif
		#if !defined(VARIANT_SEP)
			void*
		#endif
	>;

	int value_size(signed char type);
	int value_size(const Value& value);

	int int_value(const Value& value);

	signed char to_ch(int value, Err::Code overflow, Err::Code underflow);

}