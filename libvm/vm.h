#pragma once

#include <exception>

namespace vm {
	constexpr signed char ch_type { 'c' };
	constexpr signed char int_type { 'i' };
	constexpr signed char ptr_type { 'p' };

	constexpr int int_size { 5 };
	constexpr int ch_size { 2 };
	constexpr int node_size { 3 * int_size };
	constexpr int ptr_size { node_size + 1 };
	constexpr int bits_per_byte { 8 };
	constexpr int byte_mask { 0xff };
	constexpr signed char true_lit { -1 };
	constexpr signed char false_lit { 0 };
	static_assert(true_lit != false_lit);
	constexpr int heap_overhead { node_size };

	class Error: public std::exception {
	public:
		enum Code {
			#define ERR(C) err_##C,
			#include "err.def"
		};

		explicit Error(Code code): code { code } { }

		const Code code;
	};

	enum OpCode {
		#define OP(O) op_##O,
		#include "op.def"
	};

	void init(
		signed char* ram_begin, signed char* ram_end,
		const signed char* code_begin, const signed char* code_end
	);

	void step();

	const signed char* stack_begin();
	const signed char* heap_end();
	const signed char* ram_begin();
	const signed char* ram_end();
	const signed char* pc();
}
