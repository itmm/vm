#include "vm.h"

#include <iostream>

using namespace vm;

[[noreturn]] void err(Error::Code code) {
	throw Error { code };
}

namespace {
	char* ram_begin_;
	char* ram_end_;
	const char* code_begin_;
	const char* code_end_;

	const char* pc_;
	char* stack_begin_;
	char* heap_end_;

	void check_range(
		const char* begin, const char* end, Error::Code code
	) {
		if (!begin || end <= begin) { err(code); }
	}

	void has_code() {
		if (pc_ >= code_end_) { err(Error::err_leave_code_segment); }
	}

	void can_push() {
		if (stack_begin_ <= heap_end_) { err(Error::err_stack_underflow); }
	}

	void (can_pull()) {
		if (stack_begin_ >= ram_end_) { err(Error::err_stack_overflow); }
	}
}

void vm::init(
	char* ram_begin, char* ram_end,
	const char* code_begin, const char* code_end
) {
	check_range(ram_begin, ram_end, Error::err_invalid_ram);
	ram_begin_ = ram_begin; ram_end_ = ram_end;

	check_range(code_begin, code_end, Error::err_invalid_code);
	code_begin_ = code_begin; code_end_ = code_end;

	stack_begin_ = ram_end;
	heap_end_ = ram_begin;
	pc_ = code_begin;
}

void vm::step() {
	has_code();
	char op { *pc_++ };
	switch (op) {
		case op_nop: break;
		case op_break: err(Error::err_break);
		case op_push_ch: can_push(); has_code(); *--stack_begin_ = *pc_++; break;
		case op_write_char: can_pull(); std::cout << *stack_begin_++; break;
		default: err(Error::err_unknown_opcode);
	}
}