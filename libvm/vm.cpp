#include "vm.h"

#include "accessor.h"
#include "err.h"
#include "heap.h"
#include "tree.h"
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
	#if CONFIG_WITH_NUMERIC
		void fetch(Int::internal_type offset) {
			Acc::push(Acc::get_value(Stack_Ptr { Stack_Ptr::begin + offset }));
		}

		void store(Int::internal_type offset) {
			auto value { Acc::pull() };
			Acc::set_value(Stack_Ptr { Stack_Ptr::begin + offset }, value);
		}

		void jump(Int::internal_type offset, bool condition) {
			Code_Ptr target { pc + offset };
			if (condition) { pc = target; }
		}
	#endif

	#if CONFIG_WITH_CHAR
		Char negate_ch(const Char& ch) {
			return to_ch(~ch.value, Err::unexpected, Err::unexpected);
		}
	#endif

	#if CONFIG_WITH_NUMERIC
		void jump_with_stack_condition(Int::internal_type offset, bool invert) {
			bool condition { Acc::pull_internal_int() != 0 };
			if (invert) { condition = !condition; }
			jump(offset, condition);
		}
	#endif

	#if CONFIG_WITH_CALL
		Stack_Frame get_current_stack_frame() {
			if (Stack_Ptr::end == Ram_Ptr::end) {
				return Stack_Frame { };
			}
			auto top { Stack_Ptr::end };
			Temporarly_Increase_Stack_Size increase_stack_size {
				Stack_Frame::typed_size
			};
			auto value = Acc::get_value(Stack_Ptr { top });
			if (auto sf = std::get_if<Stack_Frame>(&value)) {
				return *sf;
			}
			err(Err::no_stack_frame);
		}

		void do_return() {
			auto stack_frame = get_current_stack_frame();
			while (Stack_Ptr::begin < Stack_Ptr::end) { Acc::pull(); }
			Stack_Ptr::end = Ram_Ptr::begin + stack_frame.parent.offset();
			Acc::pull();
			pc = stack_frame.pc;
		}

		#if CONFIG_WITH_EXCEPTIONS
			void set_catch_pc(const Code_Ptr& catch_pc) {
				if (Stack_Ptr::end == Ram_Ptr::end) {
					global_catch_pc = catch_pc; return;
				}
				auto stack_frame { get_current_stack_frame() };
				stack_frame.catch_pc = catch_pc;
				Stack_Ptr old_end { Stack_Ptr::end };
				{
					Temporarly_Increase_Stack_Size increase_stack_size {
						Stack_Frame::typed_size
					};
					Acc::set_value(old_end, stack_frame);
				}
			}

			void perform_throw() {
				Stack_Ptr current { Stack_Ptr::end };
				while (Ram_Ptr::begin + current.offset() < Ram_Ptr::end) {
					auto frame { get_current_stack_frame() };
					if (frame.catch_pc) { pc = frame.catch_pc; return; }
					do_return();
				}
				if (global_catch_pc) { pc = global_catch_pc; return; }
				err(Err::uncatched_throw);
			}
		#endif
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
	Ram_Ptr::begin = ram_begin_; Ram_Ptr::end = ram_end_;

	check_range(code_begin_, code_end_, Err::invalid_code);
	Code_Ptr::begin = code_begin_;
	Code_Ptr::end = code_end_;

	#if CONFIG_WITH_HEAP
		Heap_Ptr::end = Ram_Ptr::begin;
	#endif

	Stack_Ptr::begin = Ram_Ptr::end;
	#if CONFIG_WITH_CALL
		Stack_Ptr::end = Stack_Ptr::begin;
	#endif

	#if CONFIG_WITH_HEAP
		Heap::free_list = Tree { };
		Heap::alloc_list = Tree { };
	#endif

	pc = Code_Ptr { Code_Ptr::begin };
}

template<typename P> void vm::dump_block(
	const P& begin, const P& end, const char* indent
) {
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
			if (auto ch { std::get_if<Char>(&value) }) {
				std::cout << indent << current.offset() <<
					": char == " << static_cast<int>(ch->value) << "\n";
				current = current + Char::typed_size;
				continue;
			}
		#endif
		#if CONFIG_WITH_INT
			if (auto val { std::get_if<Int>(&value) }) {
				std::cout << indent << current.offset() <<
					": int == " << val->value << "\n";
				current = current + Int::typed_size;
				continue;
			}
		#endif
		#if CONFIG_WITH_HEAP
			if (auto ptr { std::get_if<Heap_Ptr>(&value) }) {
				std::cout << indent << current.offset() <<
					": ptr == " << ptr->offset() << " (" <<
					(*ptr ? (ptr->offset() - heap_overhead) : -1) << ")\n";
				current = current + ptr_size;
				continue;
			}
		#endif
		#if CONFIG_WITH_CALL
			if (auto sf = std::get_if<Stack_Frame>(&value)) {
				std::cout << indent << current.offset() << ": stack_frame {";
				std::cout << "pc == " << sf->pc.offset() << ", ";
				std::cout << "parent == " << sf->parent.offset() << ", ";
				std::cout << "outer == " << sf->outer.offset() << "}\n";
				current = current + Stack_Frame::typed_size;
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
	Stack_Ptr current { Stack_Ptr::begin };
	Stack_Ptr end { stack_upper_limit() };
	auto size { end.offset() - current.offset() };
	std::cout << "stack[" << size << "] {";
	if (size) {
		std::cout << "\n";
		dump_block(
			Stack_Ptr { Stack_Ptr::begin }, Stack_Ptr { Ram_Ptr::end }, "  "
		);
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
				auto num_args { Acc::pull_internal_int() };
				Stack_Frame sf;
				sf.pc = pc;
				sf.parent = Stack_Ptr { Stack_Ptr::end };
				sf.outer = Stack_Ptr { Stack_Ptr::end };
				Stack_Ptr position = Acc::push(sf, num_args);
				pc = new_pc;
				Stack_Ptr::end = Ram_Ptr::begin + position.offset();
				break;
			}
			#if CONFIG_WITH_EXCEPTIONS
				case op_catch: {
					Code_Ptr catch_pc { Code_Ptr::begin + Acc::get_int(pc) };
					pc = pc + Int::raw_size;
					set_catch_pc(catch_pc);
					break;
				}
			#endif
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
				Acc::push(Char { a == b ? true_lit : false_lit });
				break;
			}
			case op_fetch: fetch(Acc::pull_int().value); break;
		#endif
		#if CONFIG_WITH_HEAP
			case op_free: Heap::free_block(Acc::pull_ptr()); break;
		#endif
		#if CONFIG_WITH_INT
			case op_jmp: {
				auto value { Acc::get_int(pc).value };
				pc = pc + Int::raw_size; jump(value, true_lit); break;
			}
			case op_jmp_false: {
				auto value { Acc::get_int(pc).value };
				pc = pc + Int::raw_size;
				jump_with_stack_condition(value, true); break;
			}
			case op_jmp_true: {
				auto value { Acc::get_int(pc).value };
				pc = pc + Int::raw_size;
				jump_with_stack_condition(value, false); break;
			}
		#endif
		#if CONFIG_WITH_NUMERIC
			case op_less: {
				auto b { Acc::pull() }; auto a { Acc::pull() };
				Acc::push(Char { a < b ? true_lit : false_lit }); break;
			}
		#endif
		#if CONFIG_WITH_NUMERIC
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
		#endif
		#if CONFIG_WITH_INT
			case op_mod: { vm::ops::Mod { }(); break; }
			case op_mult: { vm::ops::Mult { }(); break; }
		#endif
		#if CONFIG_WITH_HEAP
			case op_new:
				Acc::push(Heap::alloc_block(Acc::pull_int().value)); break;
		#endif
		case op_nop: break;
		#if CONFIG_WITH_NUMERIC
			case op_not: {
				auto value { Acc::pull() };
				auto ch { std::get_if<Char>(&value) };
				if (ch) { Acc::push(negate_ch(*ch)); break; }
				auto val { std::get_if<Int>(&value) };
				if (val) { Acc::push(Int { ~val->value }); break; }
				err(Err::unknown_type);
			}
			case op_or: { vm::ops::Or { }(); break; }
		#endif
		case op_pull: Acc::pull(); break;
		#if CONFIG_WITH_CHAR
			case op_push_ch:
				Acc::push(Char { Acc::get_byte(pc) }); pc = pc + 1; break;
		#endif
		#if CONFIG_WITH_INT
			case op_push_int:
				Acc::push(Acc::get_int(pc)); pc = pc + Int::raw_size; break;
		#endif
		#if CONFIG_WITH_HEAP
			case op_receive: {
				Heap_Ptr ptr { Acc::pull_ptr() };
				ptr = ptr + Acc::pull_int().value;
				Acc::push(Acc::get_value(ptr));
				break;
			}
		#endif
		#if CONFIG_WITH_CALL
			case op_return: do_return(); break;
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
			case op_store: store(Acc::pull_int().value); break;
		#endif
		case op_swap: {
			auto a { Acc::pull() }; auto b { Acc::pull() };
			Acc::push(a); Acc::push(b); break;
		}
		#if CONFIG_WITH_EXCEPTIONS
			case op_throw: perform_throw(); break;
		#endif
		#if CONFIG_WITH_CHAR
			case op_to_ch:
				Acc::push(to_ch(
					Acc::pull_int().value, Err::to_ch_overflow,
					Err::to_ch_underflow
				));
				break;
		#endif
		#if CONFIG_WITH_INT
			case op_to_int: Acc::push(Int { Acc::pull_int() }); break;
		#endif
		#if CONFIG_WITH_CHAR
			case op_write_ch: std::cout << Acc::pull_ch().value; break;
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
