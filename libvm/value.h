#pragma once

#include <variant>

#include "defines.h"

#include "ptr.h"

namespace vm {
	#if CONFIG_WITH_INT
		struct Int {
			using value_type = CONFIG_INT_TYPE;
			static constexpr int raw_size { CONFIG_INT_SIZE };
			static constexpr int typed_size { raw_size + 1 };
			static constexpr bool use_type_field { CONFIG_INT_USE_TYPE_FIELD };
			static constexpr bool is_multi_precision {
				CONFIG_INT_MULTI_PRECISION
			};
			value_type value;
		};

		#if CONFIG_INT_MULTI_PRECISION
			bool operator==(const Int& a, const Int& b);
			bool operator<(const Int& a, const Int& b);
		#else
			inline bool operator==(const Int& a, const Int& b) {
				return a.value == b.value;
			}

			inline bool operator<(const Int& a, const Int& b) {
				return a.value < b.value;
			}
		#endif
	#endif

	#if CONFIG_WITH_CHAR
		struct Char {
			using value_type = CONFIG_CHAR_TYPE;
			static constexpr int raw_size { CONFIG_CHAR_SIZE };
			static constexpr int typed_size { raw_size + 1 };
			static constexpr bool use_type_field { CONFIG_CHAR_USE_TYPE_FIELD };
			value_type value;
		};

		inline bool operator==(const Char& a, const Char& b) {
			return a.value == b.value;
		}

		inline bool operator<(const Char& a, const Char& b) {
			return a.value < b.value;
		}

	#endif

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
	>;

	int value_size(signed char type);
	int value_size(const Value& value);

	int int_value(const Value& value);

	signed char to_ch(int value, Err::Code overflow, Err::Code underflow);

}