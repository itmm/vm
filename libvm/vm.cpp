#include "vm.h"

#include <iostream>
#include <limits>

using namespace vm;

namespace {
	signed char* ram_begin_;
	signed char* ram_end_;
	const signed char* code_begin_;
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

	inline void has_code() {
		if (pc_ >= code_end_) { err(Error::err_leave_code_segment); }
	}

	void assure_valid_ptr(
		const signed char* ptr, int size,
		const signed char* begin, const signed char* end, Error::Code code
	) {
		if (ptr < begin || ptr + size > end) { err(code); }
	}

	signed char copy_ch_from_mem(
		const signed char* mem, const signed char* begin,
		const signed char* end, Error::Code code
	) {
		assure_valid_ptr(mem, 1, begin, end, code); return *mem;
	}

	signed char copy_ch_from_stack(const signed char* mem = stack_begin_) {
		return copy_ch_from_mem(
			mem, stack_begin_, ram_end_, Error::err_leave_stack_segment
		);
	}

	signed char pull_ch() {
		auto value { copy_ch_from_stack() }; ++stack_begin_; return value;
	}

	signed char copy_ch_from_code(const signed char* mem = pc_) {
		return copy_ch_from_mem(
			mem, code_begin_, code_end_, Error::err_leave_code_segment
		);
	}

	signed char copy_ch_from_heap(const signed char* mem) {
		return copy_ch_from_mem(
			mem, ram_begin_, heap_end_, Error::err_leave_heap_segment
		);
	}

	void copy_ch_to_mem(
		signed char value, signed char* mem,
		const signed char* begin, const signed char* end, Error::Code code
	) {
		assure_valid_ptr(mem, 1, begin, end, code); *mem = value;
	}

	void copy_ch_to_stack(signed char value, signed char* mem = stack_begin_) {
		return copy_ch_to_mem(
			value, mem, stack_begin_, ram_end_, Error::err_leave_stack_segment
		);
	}

	void copy_ch_to_heap(signed char value, signed char* mem) {
		return copy_ch_to_mem(
			value, mem, ram_begin_, heap_end_, Error::err_leave_heap_segment
		);
	}

	int copy_int_from_mem(
		const signed char* mem,
		const signed char* begin, const signed char* end, Error::Code code
	) {
		assure_valid_ptr(mem, int_size, begin, end, code);
		int value { 0 };
		for (int i { int_size }; i; --i) {
			value = (value << bits_per_byte) + (*mem++ & byte_mask);
		}
		return value;
	}

	int copy_int_from_stack(const signed char* mem = stack_begin_) {
		return copy_int_from_mem(
			mem, stack_begin_, ram_end_, Error::err_leave_stack_segment
		);
	}

	int pull_int() {
		int value { copy_int_from_stack() };
		stack_begin_ += int_size;
		return value;
	}

	int copy_int_from_code(const signed char* mem = pc_) {
		return copy_int_from_mem(
			mem, code_begin_, code_end_, Error::err_leave_code_segment
		);
	}

	int copy_int_from_heap(const signed char* mem) {
		return copy_int_from_mem(
			mem, ram_begin_, heap_end_, Error::err_leave_heap_segment
		);
	}

	void copy_int_to_mem(
		int value, signed char* mem,
		const signed char* begin, const signed char *end, Error::Code code
	) {
		assure_valid_ptr(mem, int_size, begin, end, code);
		mem += int_size;
		for (int i { int_size }; i; --i) {
			*--mem = static_cast<signed char>(value);
			value >>= bits_per_byte;
		}
	}

	void copy_int_to_stack(int value, signed char* mem = stack_begin_) {
		copy_int_to_mem(
			value, mem, stack_begin_, ram_end_, Error::err_leave_stack_segment
		);
	}

	void copy_int_to_heap(int value, signed char* mem) {
		copy_int_to_mem(
			value, mem, ram_begin_, heap_end_, Error::err_leave_heap_segment
		);
	}

	void push_int(int value) {
		assure_valid_ptr(
			stack_begin_ - int_size, int_size, heap_end_, stack_begin_,
			Error::err_stack_overflow
		);
		stack_begin_ -= int_size; copy_int_to_stack(value);
	}

	void push_ch(signed char value) {
		assure_valid_ptr(
			stack_begin_ - 1, 1, heap_end_, stack_begin_,
			Error::err_stack_overflow
		);
		*--stack_begin_ = value;
	}

	void fetch_ch(int offset) {
		push_ch(copy_ch_from_stack(stack_begin_ + offset));
	}

	void store_ch(int offset) {
		auto ch { pull_ch() }; copy_ch_to_stack(ch, stack_begin_ + offset);
	}

	void fetch_int(int offset) {
		push_int(copy_int_from_stack(stack_begin_ + offset));
	}

	void store_int(int offset) {
		auto value { pull_int() };
		copy_int_to_stack(value, stack_begin_ + offset);
	}

	void jump(int offset, signed char condition) {
		const signed char* target { pc_ + offset };
		if (target < code_begin_ || target >= code_end_) {
			err(Error::err_leave_code_segment);
		}
		if (condition) { pc_ = target; }
	}

	inline signed char negate(signed char ch) {
		return static_cast<signed char>(~ch);
	}

	void jump_with_stack_condition(int offset, bool invert) {
		signed char condition { pull_ch() };
		if (invert) { condition = negate(condition); }
		jump(offset, condition);
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
		case op_jmp_ch:
			jump(pull_ch(), true_lit); break;
		case op_jeq_ch:
			jump_with_stack_condition(pull_ch(), true); break;
		case op_jne_ch:
			jump_with_stack_condition(pull_ch(), false); break;
		case op_jmp_int:
			jump(pull_int(), true_lit); break;
		case op_jeq_int:
			jump_with_stack_condition(pull_int(), true); break;
		case op_jne_int:
			jump_with_stack_condition(pull_int(), false); break;
		#if CONFIG_HAS_CH
			case op_push_ch:
				push_ch(copy_ch_from_code()); pc_++; break;

			case op_pull_ch:
				pull_ch(); break;

			case op_dup_ch: {
				auto ch { pull_ch() }; push_ch(ch); push_ch(ch); break;
			}
			case op_small_fetch_ch:
				fetch_ch(pull_ch()); break;

			case op_fetch_ch:
				fetch_ch(pull_int()); break;

			case op_small_store_ch:
				store_ch(pull_ch()); break;

			case op_store_ch:
				store_ch(pull_int()); break;

			case op_send_ch: {
				int offset { pull_int() };
				copy_ch_to_heap(pull_ch(), ram_begin_ + offset); break;
			}
			case op_receive_ch: {
				push_ch(copy_ch_from_heap(ram_begin_ + pull_int())); break;
			}
			case op_equals_ch: {
				auto b { pull_ch() }; auto a { pull_ch() };
				*--stack_begin_ = a == b ? true_lit : false_lit;
				break;
			}
			case op_less_ch: {
				auto b { pull_ch() }; auto a { pull_ch() };
				*--stack_begin_ = a < b ? true_lit : false_lit;
				break;
			}
			case op_not_ch: {
				auto value { pull_ch() };
				*--stack_begin_ = negate(value);
				break;
			}
			case op_and_ch: {
				auto b { pull_ch() }; auto a { pull_ch() };
				*--stack_begin_ = static_cast<signed char>(a & b);
				break;
			}
			case op_or_ch: {
				auto b { pull_ch() }; auto a { pull_ch() };
				*--stack_begin_ = static_cast<signed char>(a | b);
				break;
			}
			case op_xor_ch: {
				auto b { pull_ch() }; auto a { pull_ch() };
				*--stack_begin_ = static_cast<signed char>(a ^ b);
				break;
			}
			#if CONFIG_HAS_OP_WRITE_CH
				case op_write_ch:
					std::cout << pull_ch(); break;
			#endif
		#endif
		#if CONFIG_HAS_INT
			case op_pull_int:
				pull_int(); break;

			case op_add_int: {
				int b { pull_int() }; int a { pull_int() };
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
				int b { pull_int() }; int a { pull_int() };
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
				int b { pull_int() }; int a { pull_int() };
				if (a == 0x80000000 && b == -1) {
					err(Error::err_mult_int_overflow);
				}
				int value { a * b };
				if (b != 0 && value / b != a) {
					err(Error::err_mult_int_overflow);
				}
				push_int(value);
				break;
			}
			case op_div_int: {
				int b { pull_int() }; int a { pull_int() };
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
				int b { pull_int() }; int a { pull_int() };
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
				int a { pull_int() }; int b { pull_int() };
				push_int(a); push_int(b); break;
			}

			case op_small_fetch_int:
				fetch_int(pull_ch()); break;

			case op_fetch_int:
				fetch_int(pull_int()); break;

			case op_small_store_int:
				store_int(pull_ch()); break;

			case op_store_int:
				store_int(pull_int()); break;

			case op_send_int: {
				int offset { pull_int() };
				copy_int_to_heap(pull_int(), ram_begin_ + offset); break;
			}
			case op_receive_int:
				push_int(copy_int_from_heap(ram_begin_ + pull_int())); break;

			case op_equals_int: {
				int b { pull_int() }; int a { pull_int() };
				*--stack_begin_ = a == b ? true_lit : false_lit; break;
			}
			case op_less_int: {
				int b { pull_int() }; int a { pull_int() };
				*--stack_begin_ = a < b ? true_lit : false_lit; break;
			}
			case op_not_int:
				push_int(~pull_int()); break;

			case op_and_int:
				push_int(pull_int() & pull_int()); break;

			case op_or_int:
				push_int(pull_int() | pull_int()); break;

			case op_xor_int:
				push_int(pull_int() ^ pull_int()); break;

			#if CONFIG_HAS_OP_PUSH_INT
				case op_push_int:
					push_int(copy_int_from_code()); pc_ += int_size; break;
			#endif
		#endif
		#if CONFIG_HAS_CH && CONFIG_HAS_INT
			#if CONFIG_HAS_OP_CH_TO_INT
				case op_ch_to_int:
					push_int(pull_ch()); break;
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
					push_ch(static_cast<signed char>(value));
					break;
				}
			#endif
		#endif
		default: err(Error::err_unknown_opcode);
	}
}

const signed char* vm::stack_begin() { return stack_begin_; }
