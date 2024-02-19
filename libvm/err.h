#pragma once

#include <exception>

namespace vm {
	class Err: public std::exception {
		public:
			enum Code {
				#define ERR(C) C,
				#include "err.def"
			};

			explicit Err(Code code): code { code } { }

			const Code code;
	};

	[[noreturn]] void err(Err::Code code);
}
