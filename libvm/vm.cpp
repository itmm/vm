#include "vm.h"

#include <iostream>
#include <limits>

using namespace vm;

constexpr int int_size { 4 };
constexpr int bits_per_byte { 8 };
constexpr int byte_mask { 0xff };

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
		if (!begin || end < begin) { err(code); }
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
		can_push(int_size);
		for (int i { int_size }; i; --i) {
			*--stack_begin_ = static_cast<signed char>(value);
			value >>= bits_per_byte;
		}
	}

	int copy_int_from_mem(const signed char* mem, const signed char* end) {
		if (mem + int_size > end) { err(Error::err_stack_underflow); }
		int value { 0 };
		for (int i { int_size }; i; --i) {
			value = (value << bits_per_byte) + (*mem++ & byte_mask);
		}
		return value;
	}

	int copy_int_from_stack(const signed char* mem = stack_begin_) {
		return copy_int_from_mem(mem, ram_end_);
	}

	int copy_int_from_code(const signed char* mem = pc_) {
		return copy_int_from_mem(mem, code_end_);
	}

	signed char copy_ch_from_mem(const signed char* mem, const signed char* end) {
		if (mem >= end) { err(Error::err_stack_underflow); }
		return *mem;
	}

	signed char copy_ch_from_stack(const signed char* mem = stack_begin_) {
		return copy_ch_from_mem(mem, ram_end_);
	}

	int pull_int() {
		int value { copy_int_from_stack() };
		stack_begin_ += int_size;
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

			case op_pull_ch:
				can_pull(); stack_begin_++; break;

			case op_dup_ch:
				can_pull(); can_push(); --stack_begin_;
				stack_begin_[0] = stack_begin_[1]; break;

			case op_fetch_ch: {
				int value { pull_int() };
				*--stack_begin_ = copy_ch_from_stack(stack_begin_ + value);
				break;
			}
			case op_equals_ch: {
				can_pull(2);
				signed char other { *stack_begin_++ };
				*stack_begin_ = *stack_begin_ == other ? 1 : 0;
				break;
			}
			case op_less_ch: {
				can_pull(2);
				signed char other { *stack_begin_++ };
				*stack_begin_ = *stack_begin_ < other ? 1 : 0;
				break;
			}
			#if CONFIG_HAS_OP_WRITE_CH
				case op_write_ch:
					can_pull();
					std::cout << *stack_begin_++; break;
			#endif
		#endif
		#if CONFIG_HAS_INT
			case op_pull_int:
				pull_int(); break;

			case op_add_int: {
				int b { pull_int() };
				int a { pull_int() };
				if (a > 0 && b > 0 && std::numeric_limits<int>::max() - a < b) {
					err(Error::err_add_int_overflow);
				}
				if (a < 0 && b < 0 && std::numeric_limits<int>::min() - a > b) {
					err(Error::err_add_int_underflow);
				}
				push_int(a + b);
				break;
			}
			case op_sub_int: {
				int b { pull_int() };
				int a { pull_int() };
				if (a > 0 && b < 0 && a > std::numeric_limits<int>::max() + b) {
					err(Error::err_sub_int_overflow);
				}
				if (a < 0 && b > 0 && a < std::numeric_limits<int>::min() + b) {
					err(Error::err_sub_int_underflow);
				}
				push_int(a - b);
				break;
			}
			case op_mult_int: {
				int b { pull_int() };
				int a { pull_int() };
				if (a == 0x80000000 && b == -1) {
					err(Error::err_mult_int_overflow);
				}
				int value { a * b };
				if (b != 0 && value/b != a) {
					err(Error::err_mult_int_overflow);
				}
				push_int(value);
				break;
			}
			case op_div_int: {
				int b { pull_int() };
				int a { pull_int() };
				if (b == 0) { err(Error::err_div_int_divide_by_0); }
				if (a == 0x80000000 && b == -1) {
					err(Error::err_div_int_overflow);
				}
				#if CONFIG_OBERON_MATH
					int value { a / b };
					int rem { a % b };
					if (rem < 0) { value += b > 0 ? -1 : 1; }
					push_int(value);
				#else
					push_int(a / b);
				#endif
				break;
			}
			case op_mod_int: {
				int b { pull_int() };
				int a { pull_int() };
				if (b == 0) { err(Error::err_mod_int_divide_by_0); }
				#if CONFIG_OBERON_MATH
					int value { a % b };
					if (value < 0) {
						if (b > 0) { value += b; } else { value -= b; }
					}
					push_int(value);
				#else
					push_int(a % b);
				#endif
				break;
			}
			case op_dup_int:
				push_int(copy_int_from_stack()); break;

			case op_swap_int: {
				int a { pull_int() };
				int b { pull_int() };
				push_int(a); push_int(b); break;
			}
			case op_fetch_int: {
				int value { pull_int() };
				push_int(copy_int_from_stack(stack_begin_ + value)); break;
			}
			case op_equals_int: {
				int b { pull_int() }; int a { pull_int() };
				*--stack_begin_ = a == b ? 1 : 0; break;
			}
			case op_less_int: {
				int b { pull_int() }; int a { pull_int() };
				*--stack_begin_ = a < b ? 1 : 0; break;
			}
			#if CONFIG_HAS_OP_PUSH_INT
				case op_push_int:
					push_int(copy_int_from_code()); pc_ += int_size; break;
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
					}
					if (value < std::numeric_limits<signed char>::min()) {
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

