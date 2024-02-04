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
		char* ram_begin, char* ram_end,
		const char* code_begin, const char* code_end
	);

	void step();
}
