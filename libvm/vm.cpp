#include "vm.h"

#include "accessor.h"
#include "err.h"
#include "heap.h"
#include "list.h"
#include "ptr.h"
#include "value.h"
#include "ops/add.h"
#include "ops/and.h"
#include "ops/div.h"
#include "ops/mod.h"
#include "ops/mult.h"
#include "ops/or.h"
#include "ops/sub.h"
#include "ops/xor.h"

#include <iostream>
#include <set>

using namespace vm;

namespace {
	void fetch(int offset) {
		Acc::push(Acc::get_value(Stack_Ptr { stack_begin + offset }));
	}

	void store(int offset) {
		auto value { Acc::pull() };
		Acc::set_value(Stack_Ptr { stack_begin + offset }, value);
	}

	void jump(int offset, signed char condition) {
		Code_Ptr target { pc + offset };
		if (condition) { pc = target; }
	}

	signed char negate(signed char ch) {
		return to_ch(~ch, Err::unexpected, Err::unexpected);
	}

	void jump_with_stack_condition(int offset, bool invert) {
		auto condition { Acc::pull_ch() };
		if (invert) { condition = negate(condition); }
		jump(offset, condition);
	}

	Heap_Ptr find_on_free_list(int size, bool tight_fit) {
		auto current { Acc::free_list.end };
		while (current) {
			int cur_size { Acc::get_int_value(current) };
			bool found {
				tight_fit ?
					cur_size == size || cur_size > 3 * size :
					cur_size >= size
			};
			if (found) {
				int rest_size { cur_size - size };
				if (rest_size >= heap_overhead) {
					Heap_Ptr rest_block { current + size };
					Acc::set_int(rest_block, rest_size);
					Acc::set_int(current, size);
					Acc::free_list.insert(rest_block, current);
				}
				Acc::free_list.remove(current);
				return current;
			}
			current = Acc::get_ptr(current + node_prev_offset);
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
			if (heap_end + size > stack_begin) { err(Err::heap_overflow); }
			found = Heap_Ptr { heap_end };
			heap_end += size;
			Acc::set_int(found, size);
		}
		Acc::alloc_list.insert(found, Acc::alloc_list.begin);
		Acc::push(found + heap_overhead);
	}

	void free_block(Heap_Ptr block) {
		block = block - heap_overhead;
		Acc::alloc_list.remove(block);
		int size { Acc::get_int_value(block) };
		if (size < std::max(node_size, heap_overhead)) {
			err(Err::free_invalid_block);
		}
		if (Heap_Ptr { heap_end } < block + size) {
			err(Err::free_invalid_block);
		}
		Heap::insert_into_free_list(block);
	}
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
	Acc::free_list = List { };
	Acc::alloc_list = List { };
	pc = Code_Ptr { code_begin };
}

[[maybe_unused]] void vm::dump_stack() {
	std::cout << "STACK\n";
	for (auto i { stack_begin }; i != ram_end; ++i) {
		std::cout << "\t" << (int) *i << "\n";
	}
}

void vm::step() {
	signed char op { Acc::get_byte(pc) }; pc = pc + 1;
	switch (op) {
		#if CONFIG_HAS_OP_NOP
			case op_nop: break;
		#endif
		#if CONFIG_HAS_OP_BREAK
			case op_break: err(Err::got_break);
		#endif
		case op_near_jmp: {
			auto value { Acc::get_byte(pc) }; pc = pc + 1;
			jump(value, true_lit); break;
		}
		case op_near_jmp_false: {
			auto value { Acc::get_byte(pc) }; pc = pc + 1;
			jump_with_stack_condition(value, true); break;
		}
		case op_near_jmp_true: {
			auto value { Acc::get_byte(pc) }; pc = pc + 1;
			jump_with_stack_condition(value, false); break;
		}
		case op_jmp: {
			int value { Acc::get_int_value(pc) };
			pc = pc + raw_int_size; jump(value, true_lit); break;
		}
		case op_jmp_false: {
			int value { Acc::get_int_value(pc) };
			pc = pc + raw_int_size;
			jump_with_stack_condition(value, true); break;
		}
		case op_jmp_true: {
			int value { Acc::get_int_value(pc) };
			pc = pc + raw_int_size;
			jump_with_stack_condition(value, false); break;
		}

		case op_new: alloc_block(Acc::pull_int()); break;
		case op_free: free_block(Acc::pull_ptr()); break;
		case op_pull: Acc::pull(); break;

		case op_dup: {
			auto value { Acc::pull() };
			Acc::push(value); Acc::push(value); break;
		}

		case op_swap: {
			auto a { Acc::pull() }; auto b { Acc::pull() };
			Acc::push(a); Acc::push(b); break;
		}

		case op_fetch: fetch(Acc::pull_int()); break;

		case op_store: store(Acc::pull_int()); break;

		case op_send: {
			Heap_Ptr ptr { ram_begin + Acc::pull_int() };
			Acc::set_value(ptr, Acc::pull()); break;
		}

		case op_receive:
			Acc::push(Acc::get_value(Heap_Ptr { ram_begin + Acc::pull_int() }));
			break;

		case op_equals: {
			auto b { Acc::pull() }; auto a { Acc::pull() };
			Acc::push(a == b ? true_lit : false_lit);
			break;
		}

		case op_less: {
			auto b { Acc::pull() }; auto a { Acc::pull() };
			Acc::push(a < b ? true_lit : false_lit); break;
		}

		case op_not: {
			auto value { Acc::pull() };
			auto ch { std::get_if<signed char>(&value) };
			if (ch) { Acc::push(negate(*ch)); break; }
			const int* val { std::get_if<int>(&value) };
			if (val) { Acc::push(~*val); break; }
			err(Err::unknown_type);
		}

		case op_and: { vm::ops::And { }(); break; }
		case op_or: { vm::ops::Or { }(); break; }
		case op_xor: { vm::ops::Xor { }(); break; }

		#if CONFIG_HAS_CH
			case op_push_ch:
				Acc::push(Acc::get_byte(pc)); pc = pc + 1; break;

			#if CONFIG_HAS_OP_WRITE_CH
				case op_write_ch: std::cout << Acc::pull_ch(); break;
			#endif
		#endif
		case op_add: { vm::ops::Add { }(); break; }
		case op_sub: { vm::ops::Sub { }(); break; }
		case op_mult: { vm::ops::Mult { }(); break; }
		case op_div: { vm::ops::Div { }(); break; }
		case op_mod: { vm::ops::Mod { }(); break; }
		#if CONFIG_HAS_INT
			#if CONFIG_HAS_OP_PUSH_INT
				case op_push_int:
					Acc::push(Acc::get_int_value(pc));
					pc = pc + raw_int_size; break;
			#endif
		#endif
		#if CONFIG_HAS_CH && CONFIG_HAS_INT
			#if CONFIG_HAS_OP_CH_TO_INT
				case op_to_int: Acc::push(Acc::pull_int()); break;
			#endif
			#if CONFIG_HAS_OP_INT_TO_CH
				case op_to_ch:
					Acc::push(to_ch(
						Acc::pull_int(), Err::to_ch_overflow,
						Err::to_ch_underflow
					));
					break;
			#endif
		#endif
		default: err(Err::unknown_opcode);
	}
}
