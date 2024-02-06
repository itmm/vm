#pragma once

#include <exception>

namespace vm {
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
}
