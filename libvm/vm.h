#pragma once

namespace vm {
	constexpr signed char ch_type { 0x10 };
	constexpr signed char int_type { 0x20 };
	constexpr signed char ptr_type { 0x30 };
	constexpr signed char stack_frame_type { 0x40 };

	constexpr int raw_int_size { 4 }; // TODO: make size configurable
	constexpr int int_size { raw_int_size + 1 };
	constexpr int ch_size { 2 }; // TODO: make size configurable
	constexpr int node_size { 3 * raw_int_size + 1 };
	constexpr int ptr_size { raw_int_size + 1 };
	constexpr int stack_frame_size { 3 * raw_int_size + 1 };
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
