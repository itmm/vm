#pragma once

#include "defines.h"

namespace vm {
	#if CONFIG_WITH_INT
		struct Int {
			using value_type = CONFIG_INT_TYPE;
			static constexpr signed char type_ch { 0x20 };
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

	constexpr signed char ptr_type { 0x30 };
	constexpr signed char stack_frame_type { 0x40 };

	#if CONFIG_WITH_HEAP
		constexpr int node_size { 3 * Int::raw_size + 1 };
		constexpr int ptr_size { Int::raw_size + 1 };
		constexpr int heap_overhead { node_size };
	#endif
	#if CONFIG_WITH_CALL
		constexpr int stack_frame_size { 3 * Int::raw_size + 1 };
	#endif

	constexpr int bits_per_byte { 8 };
	constexpr int byte_mask { 0xff };
	constexpr signed char true_lit { -1 };
	constexpr signed char false_lit { 0 };
	static_assert(true_lit != false_lit);

	enum OpCode {
		#define OP(O) op_##O,
		#include "op.def"
	};

	void init(
		signed char* ram_begin, signed char* ram_end,
		const signed char* code_begin, const signed char* code_end
	);

	void step();

	void dump_stack();
}
