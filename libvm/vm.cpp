#include "vm.h"

#include "accessor.h"
#include "err.h"
#include "list.h"
#include "ptr.h"
#include "value.h"

#include <iostream>
#include <limits>
#include <set>

using namespace vm;

namespace {
	void fetch(int offset) {
		Accessor::push(Accessor::get_value(Stack_Ptr { stack_begin + offset }));
	}

	void store(int offset) {
		auto value { Accessor::pull() };
		Accessor::set_value(Stack_Ptr { stack_begin + offset }, value);
	}

	void jump(int offset, signed char condition) {
		Code_Ptr target { pc + offset };
		if (condition) { pc = target; }
	}

	signed char negate(signed char ch) {
		return to_ch(~ch, Err::unexpected, Err::unexpected);
	}

	void jump_with_stack_condition(int offset, bool invert) {
		auto condition { Accessor::pull_ch() };
		if (invert) { condition = negate(condition); }
		jump(offset, condition);
	}

	Heap_Ptr find_on_free_list(int size, bool tight_fit) {
		auto current { Accessor::free_list.end };
		while (current) {
			int cur_size { Accessor::get_int_value(current) };
			bool found {
				tight_fit ?
					cur_size == size || cur_size > 3 * size :
					cur_size >= size
			};
			if (found) {
				int rest_size { cur_size - size };
				if (rest_size >= heap_overhead) {
					Heap_Ptr rest_block { current + size };
					Accessor::set_int(rest_block, rest_size);
					Accessor::set_int(current, size);
					Accessor::free_list.insert(rest_block, current);
				}
				Accessor::free_list.remove(current);
				return current;
			}
			current = Accessor::get_ptr(current + node_prev_offset);
		}
		return Heap_Ptr { };
	}

	inline Heap_Ptr find_on_free_list(int size) {
		auto got { find_on_free_list(size, true) };
		if (! got) { got = find_on_free_list(size, false); }
		return got;
	}

	void alloc_block(int size) {
		size = std::max(size + heap_overhead, node_size);
		auto found { find_on_free_list(size) };
		if (!found) {
			if (heap_end + size > stack_begin) {
				err(Err::heap_overflow);
			}
			found = Heap_Ptr { heap_end };
			heap_end += size;
			Accessor::set_int(found, size);
		}
		Accessor::alloc_list.insert(found, Accessor::alloc_list.begin);
		Accessor::push(found + heap_overhead);
	}

	void free_block(Heap_Ptr block) {
		block = block - heap_overhead;
		Accessor::alloc_list.remove(block);
		int size { Accessor::get_int_value(block) };
		if (size < std::max(node_size, heap_overhead)) {
			err(Err::free_invalid_block);
		}
		if (Heap_Ptr { heap_end } < block + size) {
			err(Err::free_invalid_block);
		}
		Accessor::insert_into_free_list(block);
	}

	class Poly_Operation {
	public:
		void operator()();
		virtual ~Poly_Operation() = default;

	protected:
		Poly_Operation() = default;
		virtual void perform_ch(signed char a, signed char b) {
			err(Err::unknown_type);
		}
		virtual void perform_int(int a, int b) {
			err(Err::unknown_type);
		}
	};

	void Poly_Operation::operator()() {
		auto b { Accessor::pull() }; auto a { Accessor::pull() };
		auto a_ch = std::get_if<signed char>(&a);
		auto b_ch = std::get_if<signed char>(&b);

		if (a_ch && b_ch) { perform_ch(*a_ch, *b_ch); return; }

		auto a_int = std::get_if<int>(&a);
		auto b_int = std::get_if<int>(&b);

		if ((a_int || a_ch) && (b_int || b_ch)) {
			perform_int(a_int ? *a_int : *a_ch, b_int ? *b_int : *b_ch);
			return;
		}
		err(Err::unknown_type);
	}

	class Add_Operation: public Poly_Operation {
	public:
		Add_Operation() = default;
		void perform_ch(signed char a, signed char b) override {
			Accessor::push(to_ch(
				a + b, Err::add_overflow, Err::add_underflow
			));
		}
		void perform_int(int a, int b) override {
			if (a > 0 && b > 0 && std::numeric_limits<int>::max() - a < b) {
				err(Err::add_overflow);
			}
			if (a < 0 && b < 0 && std::numeric_limits<int>::min() - a > b) {
				err(Err::add_underflow);
			}
			Accessor::push(a + b);
		}
	};

	class Sub_Operation: public Poly_Operation {
	public:
		Sub_Operation() = default;

		void perform_ch(signed char a, signed char b) override {
			Accessor::push(to_ch(
				a - b, Err::sub_overflow, Err::sub_underflow
			));
		}
		void perform_int(int a, int b) override {
			if (a > 0 && b < 0 && a > std::numeric_limits<int>::max() + b) {
				err(Err::sub_overflow);
			}
			if (a < 0 && b > 0 && a < std::numeric_limits<int>::min() + b) {
				err(Err::sub_underflow);
			}
			Accessor::push(a - b);
		}
	};

	class Mult_Operation: public Poly_Operation {
	public:
		Mult_Operation() = default;

		void perform_ch(signed char a, signed char b) override {
			Accessor::push(to_ch(
				a * b, Err::mult_overflow, Err::mult_underflow
			));
		}
		void perform_int(int a, int b) override {
			if (a == 0x80000000 && b == -1) { err(Err::mult_overflow); }
			int value { a * b };
			if (b != 0 && value / b != a) {
				if ((a < 0 && b > 0) || (a > 0 && b < 0)) {
					err(Err::mult_underflow);
				}
				err(Err::mult_overflow);
			}
			Accessor::push(value);
		}
	};

	class Div_Operation: public Poly_Operation {
	public:
		Div_Operation() = default;

		void perform_ch(signed char a, signed char b) override {
			if (b == 0) { err(Err::div_divide_by_0); }
			Accessor::push(to_ch(
				a / b, Err::div_overflow, Err::unexpected
			));
		}
		void perform_int(int a, int b) override {
			if (b == 0) { err(Err::div_divide_by_0); }
			if (a == 0x80000000 && b == -1) { err(Err::div_overflow); }
			#if CONFIG_OBERON_MATH
				int value { a / b };
				int rem { a % b };
				if (rem < 0) { value += b > 0 ? -1 : 1; }
				Accessor::push(value);
			#else
				Accessor::push_value(a / b);
			#endif
		}
	};

	class Mod_Operation: public Poly_Operation {
	public:
		Mod_Operation() = default;

		void perform_ch(signed char a, signed char b) override {
			if (b == 0) { err(Err::mod_divide_by_0); }
			#if CONFIG_OBERON_MATH
				int value = a % b;
				if (value < 0) {
					if (b > 0) { value += b; } else { value -= b; }
				}
				Accessor::push(to_ch(
					value, Err::unexpected, Err::unexpected
				));
			#else
				Accessor::push_value(to_ch(
					a % b, Err::unexpected, Err::unexpected
				));
			#endif
		}

		void perform_int(int a, int b) override {
			if (b == 0) { err(Err::mod_divide_by_0); }
			#if CONFIG_OBERON_MATH
				int value { a % b };
				if (value < 0) {
					if (b > 0) { value += b; } else { value -= b; }
				}
				Accessor::push(value);
			#else
				Accessor::push(a % b);
			#endif
		}
	};

	class And_Operation: public Poly_Operation {
	public:
		And_Operation() = default;

		void perform_ch(signed char a, signed char b) override {
			Accessor::push(to_ch(a & b, Err::unexpected, Err::unexpected));
		}

		void perform_int(int a, int b) override { Accessor::push(a & b); }
	};

	class Or_Operation: public Poly_Operation {
	public:
		Or_Operation() = default;

		void perform_ch(signed char a, signed char b) override {
			Accessor::push(to_ch(a | b, Err::unexpected, Err::unexpected));
		}

		void perform_int(int a, int b) override { Accessor::push(a | b); }
	};

	class Xor_Operation: public Poly_Operation {
	public:
		Xor_Operation() = default;

		void perform_ch(signed char a, signed char b) override {
			Accessor::push(to_ch(a ^ b, Err::unexpected, Err::unexpected));
		}

		void perform_int(int a, int b) override { Accessor::push(a ^ b); }
	};

}

void check_range(
	const signed char* begin, const signed char* end, Err::Code code
) { if (!begin || end < begin) { err(code); } }

void vm::init(
	signed char* ram_begin_, signed char* ram_end_,
	const signed char* code_begin_, const signed char* code_end_
) {
	check_range(ram_begin_, ram_end_, Err::invalid_ram);
	ram_begin = ram_begin_; ram_end = ram_end_;

	check_range(code_begin_, code_end_, Err::invalid_code);
	code_begin = code_begin_; code_end = code_end_;

	stack_begin = ram_end;
	heap_end = ram_begin;
	Accessor::free_list = List { };
	Accessor::alloc_list = List { };
	pc = Code_Ptr { code_begin };
}

[[maybe_unused]] void vm::dump_stack() {
	std::cout << "STACK\n";
	for (auto i { stack_begin }; i != ram_end; ++i) {
		std::cout << "\t" << (int) *i << "\n";
	}
}

void vm::step() {
	signed char op { Accessor::get_byte(pc) }; pc = pc + 1;
	switch (op) {
		#if CONFIG_HAS_OP_NOP
			case op_nop: break;
		#endif
		#if CONFIG_HAS_OP_BREAK
			case op_break: err(Err::got_break);
		#endif
		case op_near_jmp: {
			auto value { Accessor::get_byte(pc) }; pc = pc + 1;
			jump(value, true_lit); break;
		}
		case op_near_jmp_false: {
			auto value { Accessor::get_byte(pc) }; pc = pc + 1;
			jump_with_stack_condition(value, true); break;
		}
		case op_near_jmp_true: {
			auto value { Accessor::get_byte(pc) }; pc = pc + 1;
			jump_with_stack_condition(value, false); break;
		}
		case op_jmp: {
			int value { Accessor::get_int_value(pc) };
			pc = pc + raw_int_size; jump(value, true_lit); break;
		}
		case op_jmp_false: {
			int value { Accessor::get_int_value(pc) };
			pc = pc + raw_int_size;
			jump_with_stack_condition(value, true); break;
		}
		case op_jmp_true: {
			int value { Accessor::get_int_value(pc) };
			pc = pc + raw_int_size;
			jump_with_stack_condition(value, false); break;
		}

		case op_new: alloc_block(Accessor::pull_int()); break;
		case op_free: free_block(Accessor::pull_ptr()); break;
		case op_pull: Accessor::pull(); break;

		case op_dup: {
			auto value { Accessor::pull() };
			Accessor::push(value); Accessor::push(value); break;
		}

		case op_swap: {
			auto a { Accessor::pull() }; auto b { Accessor::pull() };
			Accessor::push(a); Accessor::push(b); break;
		}

		case op_fetch: fetch(Accessor::pull_int()); break;

		case op_store: store(Accessor::pull_int()); break;

		case op_send: {
			Heap_Ptr ptr { ram_begin + Accessor::pull_int() };
			Accessor::set_value(ptr, Accessor::pull()); break;
		}

		case op_receive:
			Accessor::push(Accessor::get_value(
				Heap_Ptr { ram_begin + Accessor::pull_int() }
			)); break;

		case op_equals: {
			auto b { Accessor::pull() }; auto a { Accessor::pull() };
			Accessor::push(a == b ? true_lit : false_lit);
			break;
		}

		case op_less: {
			auto b { Accessor::pull() }; auto a { Accessor::pull() };
			Accessor::push(a < b ? true_lit : false_lit); break;
		}

		case op_not: {
			auto value { Accessor::pull() };
			auto ch { std::get_if<signed char>(&value) };
			if (ch) { Accessor::push(negate(*ch)); break; }
			const int* val { std::get_if<int>(&value) };
			if (val) { Accessor::push(~*val); break; }
			err(Err::unknown_type);
		}

		case op_and: { And_Operation { }(); break; }
		case op_or: { Or_Operation { }(); break; }
		case op_xor: { Xor_Operation { }(); break; }

		#if CONFIG_HAS_CH
			case op_push_ch:
				Accessor::push(Accessor::get_byte(pc)); pc = pc + 1; break;

			#if CONFIG_HAS_OP_WRITE_CH
				case op_write_ch: std::cout << Accessor::pull_ch(); break;
			#endif
		#endif
		case op_add: { Add_Operation { }(); break; }
		case op_sub: { Sub_Operation { }(); break; }
		case op_mult: { Mult_Operation { }(); break; }
		case op_div: { Div_Operation { }(); break; }
		case op_mod: { Mod_Operation { }(); break; }
		#if CONFIG_HAS_INT
			#if CONFIG_HAS_OP_PUSH_INT
				case op_push_int:
					Accessor::push(Accessor::get_int_value(pc));
					pc = pc + raw_int_size; break;
			#endif
		#endif
		#if CONFIG_HAS_CH && CONFIG_HAS_INT
			#if CONFIG_HAS_OP_CH_TO_INT
				case op_to_int:
					Accessor::push(Accessor::pull_int()); break;
			#endif
			#if CONFIG_HAS_OP_INT_TO_CH
				case op_to_ch:
					Accessor::push(to_ch(
						Accessor::pull_int(), Err::to_ch_overflow,
						Err::to_ch_underflow
					));
					break;
			#endif
		#endif
		default: err(Err::unknown_opcode);
	}
}
