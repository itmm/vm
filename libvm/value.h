#pragma once

#include <variant>

#include "defines.h"

#include "ptr.h"

namespace vm {
	#if CONFIG_WITH_NUMERIC
		struct Int {
			using internal_type = CONFIG_INTERNAL_INT_TYPE;

			#if CONFIG_WITH_INT
				using value_type = CONFIG_INT_TYPE;
				static constexpr signed char type_ch { 0x20 };
				static constexpr int raw_size { CONFIG_INT_SIZE };
				static constexpr int typed_size { raw_size + 1 };
				static constexpr bool use_type_field {
					CONFIG_INT_USE_TYPE_FIELD
				};
				static constexpr bool is_multi_precision {
					CONFIG_INT_MULTI_PRECISION
				};
				value_type value { 0 };
			#else
				Int() = delete;
			#endif
		};
	#endif

	#if CONFIG_WITH_INT
		template<typename P> P operator+(const P& ptr, const Int& offset) {
			return ptr + offset.value;
		}

		template<typename P> P operator-(const P& ptr, const Int& offset) {
			return ptr - offset.value;
		}

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
			static constexpr signed char type_ch { 0x10 };
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
			Stack_Frame() = default;
			Code_Ptr pc { Code_Ptr::end };
			#if CONFIG_WITH_EXCEPTIONS
				Code_Ptr catch_pc { };
			#endif
			Stack_Ptr parent { };
			Stack_Ptr outer { };

			static constexpr signed char type_ch { 0x40 };
			#if CONFIG_WITH_EXCEPTIONS
				static constexpr int raw_size { 3 * Int::raw_size };
			#else
				static constexpr int raw_size { 2 * Int::raw_size };
			#endif
			static constexpr int typed_size { raw_size + 1 };
		};

		bool operator==(const Stack_Frame& a, const Stack_Frame& b);
		bool operator<(const Stack_Frame& a, const Stack_Frame& b);
	#endif

	using Value = std::variant<
		#if CONFIG_WITH_CHAR
			#define VARIANT_SEP 1
			Char
		#endif
		#if CONFIG_WITH_INT
			#if !defined(VARIANT_SEP)
				#define VARIANT_SEP 1
			#else
				,
			#endif
			Int
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
			nullptr_t
		#endif
	>;

	int value_size(signed char type);
	int value_size(const Value& value);

	#if CONFIG_WITH_NUMERIC
		Int::internal_type internal_int_value(const Value& value);
	#endif

	#if CONFIG_WITH_INT
		#if CONFIG_INTERNAL_INT_IS_INT
			inline Int int_value(const Value& value) {
				return Int { internal_int_value(value) };
			}
		#else
			Int int_value(const Value& value);
		#endif
	#endif

	#if CONFIG_WITH_CHAR
		Char to_ch(int value, Err::Code overflow, Err::Code underflow);
	#endif
}