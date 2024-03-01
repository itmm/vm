#pragma once

#include "defines.h"
#include "value.h"

namespace vm {
	constexpr signed char ptr_type { 0x30 };

	#if CONFIG_WITH_HEAP
		constexpr int node_size { 3 * Int::raw_size };
		constexpr int ptr_size { Int::raw_size + 1 };
		constexpr int heap_overhead { node_size };
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

	template<typename P> void dump_block(const P& begin, const P& end, const char* indent);

	void dump_stack();
}
