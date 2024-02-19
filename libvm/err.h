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

	[[noreturn]] void err(Error::Code code);
}
