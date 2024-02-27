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

using namespace vm;

namespace {
	void fetch(int offset) {
		Acc::push(Acc::get_value(Stack_Ptr { stack_begin + offset }));
	}

	void store(int offset) {
		auto value { Acc::pull() };
		Acc::set_value(Stack_Ptr { stack_begin + offset }, value);
	}

	void jump(int offset, int condition) {
		Code_Ptr target { pc + offset };
		if (condition) { pc = target; }
	}

	#if CONFIG_WITH_CHAR
		signed char negate_ch(signed char ch) {
			return to_ch(~ch, Err::unexpected, Err::unexpected);
		}
	#endif

	#if CONFIG_WITH_NUMERIC
		void jump_with_stack_condition(int offset, bool invert) {
			auto condition { Acc::pull_int() };
			if (invert) { condition = ~condition; }
			jump(offset, condition);
		}
	#endif
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
	old_code_begin = Code_Ptr::begin = code_begin_;
	old_code_end = Code_Ptr::end = code_end_;

	#if CONFIG_WITH_HEAP
		heap_end = ram_begin;
	#endif

	stack_begin = ram_end;
	#if CONFIG_WITH_CALL
		stack_end = stack_begin;
	#endif

	#if CONFIG_WITH_HEAP
		Heap::free_list = List { };
		Heap::alloc_list = List { };
	#endif

	pc = Code_Ptr { Code_Ptr::begin };
}

template<typename P> void vm::dump_block(const P& begin, const P& end, const char* indent) {
	P current { begin };
	while (current < end) {
		vm::Value value;
		try { value = Acc::get_value(current); }
		catch (const Err& err) {
			std::cout << indent << "! NO VALID VALUE AT " <<
				current.offset() << "\n";
			break;
		}
		#if CONFIG_WITH_CHAR
			if (auto ch { std::get_if<signed char>(&value) }) {
				std::cout << indent << current.offset() <<
					": char == " << static_cast<int>(*ch) << "\n";
				current = current + Char::typed_size;
				continue;
			}
		#endif
		#if CONFIG_WITH_INT
			if (auto val { std::get_if<int>(&value) }) {
				std::cout << indent << current.offset() <<
					": int == " << *val << "\n";
				current = current + Int::typed_size;
				continue;
			}
		#endif
		#if CONFIG_WITH_HEAP
			if (auto ptr { std::get_if<Heap_Ptr>(&value) }) {
				std::cout << indent << current.offset() << ": ptr == " << ptr->offset() << " ("
					<< (*ptr ? (ptr->offset() - heap_overhead) : -1) << ")\n";
				current = current + ptr_size;
				continue;
			}
		#endif
		#if CONFIG_WITH_CALL
			if (std::get_if<Stack_Frame>(&value)) {
				// TODO: log stack frame details
				std::cout << indent << current.offset() << ": stack_frame ()\n";
				current = current + stack_frame_size;
				continue;
			}
		#endif
		std::cout << indent << "! UNKNOWN TYPE AT " << current.offset() << "\n";
		break;
	}
	if (!(current == end)) {
		std::cout << indent << "! INVALID END " << current.offset()<< " != " <<
			end.offset() << "\n";
	}
}

void vm::dump_stack() {
	Stack_Ptr current { stack_begin };
	Stack_Ptr end { stack_upper_limit() };
	int size { end.offset() - current.offset() };
	std::cout << "stack[" << size << "] {";
	if (size) {
		std::cout << "\n";
		dump_block(Stack_Ptr { stack_begin }, Stack_Ptr { ram_end }, "  ");
		std::cout << "}\n";
	} else { std::cout << " }\n"; }
}

void vm::step() {
	// dump_stack();
	signed char op { Acc::get_byte(pc) }; pc = pc + 1;
	switch (op) {
		#if CONFIG_WITH_NUMERIC
			case op_add: { vm::ops::Add { }(); break; }
			case op_and: { vm::ops::And { }(); break; }
		#endif
		case op_break: err(Err::got_break);
		#if CONFIG_WITH_CALL
			case op_call: {
				Code_Ptr new_pc { Code_Ptr::begin + Acc::pull_int() };
				int num_args { Acc::pull_int() };
				Stack_Frame sf;
				sf.pc = pc;
				sf.parent = Ram_Ptr { stack_end };
				sf.outer = Ram_Ptr { stack_end };
				Stack_Ptr position = Acc::push(sf, num_args);
				pc = new_pc;
				stack_end = stack_begin + position.offset();
				break;
			}
		#endif
		#if CONFIG_WITH_GC
			case op_collect_garbage: Heap::collect_garbage(); break;
		#endif
		#if CONFIG_WITH_INT
			case op_div: { vm::ops::Div { }(); break; }
		#endif
		case op_dup: {
			auto value { Acc::pull() };
			Acc::push(value); Acc::push(value); break;
		}
		#if CONFIG_WITH_NUMERIC
			case op_equals: {
				auto b { Acc::pull() }; auto a { Acc::pull() };
				Acc::push(a == b ? true_lit : false_lit);
				break;
			}
			case op_fetch: fetch(Acc::pull_int()); break;
		#endif
		#if CONFIG_WITH_HEAP
			case op_free: Heap::free_block(Acc::pull_ptr()); break;
		#endif
		#if CONFIG_WITH_INT
			case op_jmp: {
				int value { Acc::get_int(pc) };
				pc = pc + Int::raw_size; jump(value, true_lit); break;
			}
			case op_jmp_false: {
				int value { Acc::get_int(pc) };
				pc = pc + Int::raw_size;
				jump_with_stack_condition(value, true); break;
			}
			case op_jmp_true: {
				int value { Acc::get_int(pc) };
				pc = pc + Int::raw_size;
				jump_with_stack_condition(value, false); break;
			}
		#endif
		#if CONFIG_WITH_NUMERIC
			case op_less: {
				auto b { Acc::pull() }; auto a { Acc::pull() };
				Acc::push(a < b ? true_lit : false_lit); break;
			}
		#endif
		case op_near_jmp: {
			auto value { Acc::get_byte(pc) }; pc = pc + 1;
			jump(value, true_lit); break;
		}
		#if CONFIG_WITH_NUMERIC
			case op_near_jmp_false: {
				auto value { Acc::get_byte(pc) }; pc = pc + 1;
				jump_with_stack_condition(value, true); break;
			}
			case op_near_jmp_true: {
				auto value { Acc::get_byte(pc) }; pc = pc + 1;
				jump_with_stack_condition(value, false); break;
			}
		#endif
		#if CONFIG_WITH_INT
			case op_mod: { vm::ops::Mod { }(); break; }
			case op_mult: { vm::ops::Mult { }(); break; }
		#endif
		#if CONFIG_WITH_HEAP
			case op_new: Heap::alloc_block(Acc::pull_int()); break;
		#endif
		case op_nop: break;
		#if CONFIG_WITH_NUMERIC
			case op_not: {
				auto value { Acc::pull() };
				auto ch { std::get_if<signed char>(&value) };
				if (ch) { Acc::push(negate_ch(*ch)); break; }
				const int* val { std::get_if<int>(&value) };
				if (val) { Acc::push(~*val); break; }
				err(Err::unknown_type);
			}
			case op_or: { vm::ops::Or { }(); break; }
		#endif
		case op_pull: Acc::pull(); break;
		#if CONFIG_WITH_CHAR
			case op_push_ch:
				Acc::push(Acc::get_byte(pc)); pc = pc + 1; break;
		#endif
		#if CONFIG_WITH_INT
			case op_push_int:
				Acc::push(Acc::get_int(pc));
				pc = pc + Int::raw_size; break;
		#endif
		#if CONFIG_WITH_HEAP
			case op_receive: {
				Heap_Ptr ptr { Acc::pull_ptr() };
				ptr = ptr + Acc::pull_int();
				Acc::push(Acc::get_value(ptr));
				break;
			}
		#endif
		#if CONFIG_WITH_CALL
			case op_return: {
				auto value { Acc::get_value(Ram_Ptr { stack_end }) };
				if (auto sf = std::get_if<Stack_Frame>(&value)) {
					while (stack_begin < stack_end) { Acc::pull(); }
					stack_end = ram_begin + sf->parent.offset();
					Acc::pull();
					pc = sf->pc;
				} else { err(Err::no_stack_frame); }
				break;
			}
		#endif
		#if CONFIG_WITH_HEAP
			case op_send: {
				Heap_Ptr ptr { Acc::pull_ptr() };
				ptr = ptr + Acc::pull_int();
				Acc::set_value(ptr, Acc::pull()); break;
			}
		#endif
		#if CONFIG_WITH_NUMERIC
			case op_sub: { vm::ops::Sub { }(); break; }
			case op_store: store(Acc::pull_int()); break;
		#endif
		case op_swap: {
			auto a { Acc::pull() }; auto b { Acc::pull() };
			Acc::push(a); Acc::push(b); break;
		}
		#if CONFIG_WITH_CHAR
			case op_to_ch:
				Acc::push(to_ch(
					Acc::pull_int(), Err::to_ch_overflow,
					Err::to_ch_underflow
				));
				break;
		#endif
		#if CONFIG_WITH_INT
			case op_to_int: Acc::push(Acc::pull_int()); break;
		#endif
		#if CONFIG_WITH_CHAR
			case op_write_ch: std::cout << Acc::pull_ch(); break;
		#endif
		#if CONFIG_WITH_NUMERIC
			case op_xor: { vm::ops::Xor { }(); break; }
		#endif
		default: err(Err::unknown_opcode);
	}
}

// instantiate templates

#if CONFIG_WITH_HEAP
	template void vm::dump_block(const Heap_Ptr&, const Heap_Ptr&, const char*);
#endif
