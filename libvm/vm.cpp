#include "vm.h"

#include <iostream>
#include <limits>

using namespace vm;

namespace {
	[[maybe_unused]] signed char* ram_begin_;
	signed char* ram_end_;
	[[maybe_unused]] const signed char* code_begin_;
	const signed char* code_end_;

	const signed char* pc_;
	signed char* stack_begin_;
	signed char* heap_end_;

	[[noreturn]] void err(Error::Code code) {
		throw Error { code };
	}

	void check_range(
		const signed char* begin, const signed char* end, Error::Code code
	) {
		if (!begin || end <= begin) { err(code); }
	}

	void has_code(int count = 1) {
		if (pc_ + count > code_end_) { err(Error::err_leave_code_segment); }
	}

	void can_push(int count = 1) {
		if (stack_begin_ < heap_end_ + count) {
			err(Error::err_stack_overflow);
		}
	}

	void (can_pull(int count = 1)) {
		if (stack_begin_ + count > ram_end_) {
			err(Error::err_stack_underflow);
		}
	}

	void push_int(int value) {
		can_push(4);
		for (int i { 4 }; i; --i) {
			*--stack_begin_ = static_cast<signed char>(value);
			value >>= 8;
		}
	}

	int copy_int_from_mem(const signed char* mem) {
		int value { 0 };
		for (int i { 4 }; i; --i) {
			value = (value << 8) + (*mem++ & 0xff);
		}
		return value;
	}

	int pull_int() {
		can_pull(4);
		int value { copy_int_from_mem(stack_begin_) };
		stack_begin_ += 4;
		return value;
	}
}

void vm::init(
	signed char* ram_begin, signed char* ram_end,
	const signed char* code_begin, const signed char* code_end
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
	signed char op { *pc_++ };
	switch (op) {
		#if CONFIG_HAS_OP_NOP
			case op_nop: break;
		#endif
		#if CONFIG_HAS_OP_BREAK
			case op_break: err(Error::err_break);
		#endif
		#if CONFIG_HAS_CH
			case op_push_ch:
				can_push(); has_code();
				*--stack_begin_ = *pc_++; break;

			#if CONFIG_HAS_OP_WRITE_CH
				case op_write_ch:
					can_pull();
					std::cout << *stack_begin_++; break;
			#endif
		#endif
		#if CONFIG_HAS_INT
			case op_add_int: {
				int a { pull_int() };
				int b { pull_int() };
				if (a > 0 && b > 0 && std::numeric_limits<int>::max() - a < b) {
					err(Error::err_add_int_overflow);
				} else if (a < 0 && b < 0 && std::numeric_limits<int>::min() - a > b) {
					err(Error::err_add_int_underflow);
				}
				push_int(a + b);
				break;
			}
			#if CONFIG_HAS_OP_PUSH_INT
			case op_push_int: {
				has_code(4);
				int value { copy_int_from_mem(pc_) }; pc_ += 4;
				push_int(value);
				break;
			}
			#endif
		#endif
		#if CONFIG_HAS_CH && CONFIG_HAS_INT
			#if CONFIG_HAS_OP_CH_TO_INT
				case op_ch_to_int:
					can_pull();
					push_int(*stack_begin_++);
					break;
			#endif
			#if CONFIG_HAS_OP_INT_TO_CH
				case op_int_to_ch: {
					int value { pull_int() };
					if (value > std::numeric_limits<signed char>::max()) {
						err(Error::err_int_to_ch_overflow);
					} else if (value < std::numeric_limits<signed char>::min()) {
						err(Error::err_int_to_ch_underflow);
					}
					can_push();
					*--stack_begin_ = static_cast<signed char>(value);
					break;
				}
			#endif
		#endif
		default: err(Error::err_unknown_opcode);
	}
}

const signed char* vm::stack_begin() { return stack_begin_; }

