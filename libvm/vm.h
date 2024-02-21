#pragma once

namespace vm {
	constexpr signed char ch_type { 'c' };
	constexpr signed char int_type { 'i' };
	constexpr signed char ptr_type { 'p' };

	constexpr int raw_int_size { 4 };
	constexpr int int_size { raw_int_size + 1 };
	constexpr int ch_size { 2 };
	constexpr int node_size { 3 * raw_int_size };
	constexpr int ptr_size { raw_int_size + 1 };
	constexpr int bits_per_byte { 8 };
	constexpr int byte_mask { 0xff };
	constexpr signed char true_lit { -1 };
	constexpr signed char false_lit { 0 };
	static_assert(true_lit != false_lit);
	constexpr int heap_overhead { node_size };

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
