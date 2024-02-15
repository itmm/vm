#include "vm.h"

#include <iostream>
#include <limits>
#include <set>

using namespace vm;

namespace {
	[[noreturn]] void err(Error::Code code) { throw Error { code }; }

	const signed char* code_begin_;
	const signed char* code_end_;

	template<typename T, T& B, T& E, Error::Code C>
	class Const_Ptr {
		public:
			explicit Const_Ptr(T ptr = nullptr): ptr_ { ptr } {
				if (ptr_) { check(0); }
			}

			[[nodiscard]] signed char get_ch();
			[[nodiscard]] int get_int() const;

			[[nodiscard]] T get() const { return ptr_; }

			explicit operator bool() const { return ptr_; }

		protected:
			T ptr_;

			void check(int size) const;
	};

	template<typename T, T& B, T& E, Error::Code C>
	void Const_Ptr<T, B, E, C>::check(int size) const {
		if (ptr_ < B || ptr_ + size > E) { err(C); }
	}

	template<typename T, T& B, T& E, Error::Code C>
	signed char Const_Ptr<T, B, E, C>::get_ch() {
		check(ch_size); return *ptr_;
	}

	template<typename T, T& B, T& E, Error::Code C>
	int Const_Ptr<T, B, E, C>::get_int() const {
		check(int_size);
		int value { 0 };
		for (auto i { ptr_ }, e { ptr_ + int_size }; i < e; ++i) {
			value = (value << bits_per_byte) + (*i & byte_mask);
		}
		return value;
	}

	template<typename T, T& B, T& E, Error::Code C>
	Const_Ptr<T, B, E, C> operator+(
		const Const_Ptr<T, B, E, C>& ptr, int offset
	) {
		return Const_Ptr<T, B, E, C> { ptr.get() + offset };
	}

	template<typename T, T& B, T& E, Error::Code C>
	Const_Ptr<T, B, E, C> operator-(
		const Const_Ptr<T, B, E, C>& ptr, int offset
	) {
		return Const_Ptr<T, B, E, C> { ptr.get() - offset };
	}

	template<typename T, T& B, T& E, Error::Code C>
	bool operator==(
		const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
	) {
		return a.get() == b.get();
	}

	template<typename T, T& B, T& E, Error::Code C>
	bool operator<(
		const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
	) {
		return a.get() < b.get();
	}

	using Code_Ptr = Const_Ptr<
	    const signed char*, code_begin_, code_end_,
		Error::err_leave_code_segment
	>;

	Code_Ptr pc_;

	template<typename T, T& B, T& E, Error::Code C>
	class Ptr : public Const_Ptr<T, B, E, C> {
		public:
			explicit Ptr(T ptr = nullptr): Const_Ptr<T, B, E, C>(ptr) { }

			void set_ch(signed char value);
			void set_int(int value);
	};

	template<typename T, T& B, T& E, Error::Code C>
	void Ptr<T, B, E, C>::set_ch(signed char value) {
		this->check(ch_size); *this->ptr_ = value;
	}

	template<typename T, T& B, T& E, Error::Code C>
	void Ptr<T, B, E, C>::set_int(int value) {
		this->check(int_size);
		for (
			auto i { this->ptr_ + int_size - 1 }, e { this->ptr_ }; i >= e; --i
		) {
			*i = static_cast<signed char>(value);
			value >>= bits_per_byte;
		}
	}

	template<typename T, T& B, T& E, Error::Code C>
	Ptr<T, B, E, C> operator+(const Ptr<T, B, E, C>& ptr, int offset) {
		return Ptr<T, B, E, C>(ptr.get() + offset);
	}

	template<typename T, T& B, T& E, Error::Code C>
	Ptr<T, B, E, C> operator-(const Ptr<T, B, E, C>& ptr, int offset) {
		return Ptr<T, B, E, C>(ptr.get() - offset);
	}

	signed char* ram_begin_;
	signed char* heap_end_;
	signed char* stack_begin_;
	signed char* ram_end_;

	using Heap_Ptr = Ptr<
		signed char*, ram_begin_, heap_end_, Error::err_leave_heap_segment
	>;

	using Stack_Ptr = Ptr<
		signed char*, stack_begin_, ram_end_, Error::err_leave_stack_segment
	>;

	Heap_Ptr get_ptr(Heap_Ptr ptr) {
		int value { ptr.get_int() };
		return Heap_Ptr { value >= 0 ? ram_begin_ + value : nullptr };
	}

	void set_ptr(Heap_Ptr ptr, const Heap_Ptr value) {
		auto got { value.get() };
		ptr.set_int(got ? static_cast<int>(got - ram_begin_) : -1);
	}

	Heap_Ptr free_list_;

	// TODO: separate call stack or stack guard
	// TODO: tree of allocated blocks
	// TODO: references to each block

	signed char pull_ch() {
		auto value { Stack_Ptr { stack_begin_ }.get_ch() };
		++stack_begin_; return value;
	}

	int pull_int() {
		int value { Stack_Ptr { stack_begin_ }.get_int() };
		stack_begin_ += int_size;
		return value;
	}

	void push_int(int value) {
		if (heap_end_ + int_size > stack_begin_) {
			err(Error::err_stack_overflow);
		}
		stack_begin_ -= int_size; Stack_Ptr ptr { stack_begin_ };
		ptr.set_int(value);
	}

	void push_ch(signed char value) {
		if (heap_end_ + ch_size > stack_begin_) {
			err(Error::err_stack_overflow);
		}
		stack_begin_ -= ch_size; Stack_Ptr ptr { stack_begin_ };
		ptr.set_ch(value);
	}

	void fetch_ch(int offset) {
		push_ch(Stack_Ptr { stack_begin_ + offset }.get_ch());
	}

	void store_ch(int offset) {
		auto ch { pull_ch() }; Stack_Ptr { stack_begin_ + offset}.set_ch(ch);
	}

	void fetch_int(int offset) {
		push_int(Stack_Ptr { stack_begin_ + offset }.get_int());
	}

	void store_int(int offset) {
		auto value { pull_int() };
		Stack_Ptr { stack_begin_ + offset }.set_int(value);
	}

	void jump(int offset, signed char condition) {
		Code_Ptr target { pc_ + offset };
		if (condition) { pc_ = target; }
	}

	inline signed char negate(signed char ch) {
		return static_cast<signed char>(~ch);
	}

	void jump_with_stack_condition(int offset, bool invert) {
		auto condition { pull_ch() };
		if (invert) { condition = negate(condition); }
		jump(offset, condition);
	}

	void chain_in_free_list(const Heap_Ptr& next, Heap_Ptr pre) {
		if (pre) { set_ptr(pre + int_size, next); }
		else { free_list_ = next; }
	}

	Heap_Ptr find_on_free_list(int size, bool tight_fit) {
		Heap_Ptr previous;
		auto current { free_list_ };
		while (current) {
			int cur_size { current.get_int() };
			auto next { get_ptr(current + int_size) };
			bool found {
				tight_fit ?
					cur_size == size || cur_size > 3 * size :
					cur_size >= size
			};
			if (found) {
				int rest_size { cur_size - size };
				if (rest_size >= 2 * int_size) {
					Heap_Ptr rest_block { current + size };
					rest_block.set_int(rest_size);
					current.set_int(size);
					chain_in_free_list(next, rest_block);
					chain_in_free_list(rest_block, previous);
				} else { chain_in_free_list(next, previous); }
				return current;
			}
			previous = current; current = next;
		}
		return Heap_Ptr { };
	}

	inline Heap_Ptr find_on_free_list(int size) {
		auto got { find_on_free_list(size, true) };
		if (! got) { got = find_on_free_list(size, false); }
		return got;
	}

	void alloc_block(int size) {
		size = std::max(size + int_size, 2 * int_size);
		auto found { find_on_free_list(size) };
		if (!found) {
			if (heap_end_ + size > stack_begin_) {
				err(Error::err_heap_overflow);
			}
			found = Heap_Ptr { heap_end_ };
			heap_end_ += size;
			found.set_int(size);
		}
		push_int(static_cast<int>(found.get() - ram_begin_) + int_size);
	}

	inline Heap_Ptr get_previous(const Heap_Ptr& node) {
		Heap_Ptr previous;
		auto current { free_list_};
		for (;;) {
			if (node == current) { return previous; }
			if (!current || current < node) {
				err(Error::err_free_invalid_block);
			}
			previous = current;
			current = get_ptr(current + int_size);
		}
	}

	inline void insert_into_free_list(Heap_Ptr block) {
		Heap_Ptr greater { };
		Heap_Ptr smaller { free_list_ };
		while (smaller && block < smaller) {
			greater = smaller;
			smaller = get_ptr(smaller + int_size);
		}
		int size { block.get_int() };

		if (smaller) {
			int smaller_size { smaller.get_int() };
			if (smaller + smaller_size == block) {
				smaller.set_int(smaller_size + size);
				block = smaller; size += smaller_size;
			} else { set_ptr(block + int_size, smaller); }
		} else { set_ptr(block + int_size, smaller); }

		if (greater) {
			if (block + size == greater) {
				block.set_int(size + greater.get_int());
				greater = get_previous(greater);
				if (greater) { set_ptr(greater + int_size, block); }
			} else { set_ptr(greater + int_size, block); }
		}

		if (block.get() + size == heap_end_) {
			heap_end_ = block.get(); block = Heap_Ptr { };
			if (greater) { set_ptr(greater + int_size, block); }
		}
		if (! greater) { free_list_ = block; }
	}

	void free_block(Heap_Ptr block) {
		block = block - int_size;
		int size { block.get_int() };
		if (size < 2 * int_size) { err(Error::err_free_invalid_block); }
		if (block.get() + size > heap_end_) {
			err(Error::err_free_invalid_block);
		}
		insert_into_free_list(block);
	}
}

void check_range(
	const signed char* begin, const signed char* end, Error::Code code
) {
	if (!begin || end < begin) { err(code); }
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
	free_list_ = Heap_Ptr { };
	pc_ = Code_Ptr { code_begin };
}

void vm::step() {
	signed char op { pc_.get_ch() }; pc_ = pc_ + ch_size;
	switch (op) {
		#if CONFIG_HAS_OP_NOP
			case op_nop: break;
		#endif
		#if CONFIG_HAS_OP_BREAK
			case op_break: err(Error::err_break);
		#endif
		case op_small_jmp: {
			auto value { pc_.get_ch() }; pc_ = pc_ + ch_size;
			jump(value, true_lit); break;
		}
		case op_small_jmp_false: {
			auto value { pc_.get_ch() }; pc_ = pc_ + ch_size;
			jump_with_stack_condition(value, true); break;
		}
		case op_small_jmp_true: {
			auto value { pc_.get_ch() }; pc_ = pc_ + ch_size;
			jump_with_stack_condition(value, false); break;
		}
		case op_jmp: {
			int value { pc_.get_int() }; pc_ = pc_ + int_size;
			jump(value, true_lit); break;
		}
		case op_jmp_false: {
			int value { pc_.get_int() }; pc_ = pc_ + int_size;
			jump_with_stack_condition(value, true); break;
		}
		case op_jmp_true: {
			int value { pc_.get_int() }; pc_ = pc_ + int_size;
			jump_with_stack_condition(value, false); break;
		}
		case op_small_new:
			alloc_block(pull_ch()); break;

		case op_new:
			alloc_block(pull_int()); break;

		case op_free:
			free_block(Heap_Ptr { ram_begin_ + pull_int() }); break;

		#if CONFIG_HAS_CH
			case op_push_ch:
				push_ch(pc_.get_ch()); pc_ = pc_ + 1; break;

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

			case op_send_ch: { // TODO: add unit-tests
				Heap_Ptr ptr { ram_begin_ + pull_int() };
				ptr.set_ch(pull_ch()); break;
			}
			case op_receive_ch: { // TODO: add unit-tests
				push_ch(Heap_Ptr { ram_begin_ + pull_int() }.get_ch()); break;
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
				push_int(Stack_Ptr { stack_begin_ }.get_int()); break;

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

			case op_send_int: { // TODO: add unit-tests
				int offset { pull_int() };
				Heap_Ptr { ram_begin_ + offset }.set_int(pull_int()); break;
			}
			case op_receive_int: // TODO: add unit-tests
				push_int(Heap_Ptr { ram_begin_ + pull_int() }.get_int()); break;

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
					push_int(pc_.get_int()); pc_ = pc_ + int_size; break;
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
const signed char* vm::heap_end() { return heap_end_; }
const signed char* vm::ram_begin() { return ram_begin_; }
const signed char* vm::ram_end() { return ram_end_; }
const signed char* vm::pc() { return pc_.get(); }
