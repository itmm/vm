#include "vm.h"

#include <iostream>
#include <limits>
#include <set>

using namespace vm;

namespace {
	[[noreturn]] void err(Error::Code code) { throw Error { code }; }

	const signed char* code_begin_;
	const signed char* code_end_;

	using Value = std::variant<signed char, int, signed char*>;

	int value_size(const Value& value) {
		if (std::get_if<signed char>(&value)) {
			return ch_size;
		} else if (std::get_if<int>(&value)) {
			return int_size;
		} else if (std::get_if<signed char*>(&value)) {
			return ptr_size;
		} else { err(Error::err_unknown_type); }
	}

	template<typename T, T& B, T& E, Error::Code C>
	class Const_Ptr {
		public:
			explicit Const_Ptr(T ptr = nullptr): ptr_ { ptr } {
				if (ptr_) { check(0); }
			}

			[[nodiscard]] signed char get_byte();
			[[nodiscard]] Value get_value();
			[[nodiscard]] signed char get_ch();
			[[nodiscard]] int get_int() const;

			[[nodiscard]] T get_raw() const { return ptr_; }

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
	signed char Const_Ptr<T, B, E, C>::get_byte() {
		check(1); return *ptr_;
	}

	template<typename T, T& B, T& E, Error::Code C>
	signed char Const_Ptr<T, B, E, C>::get_ch() {
		check(ch_size); if (*ptr_ != ch_type) { err(Error::err_no_char); }
		return ptr_[1];
	}

	template<typename T> int get_int_value(T* ptr) {
		int value { 0 };
		for (auto i { ptr }, e { ptr + raw_int_size }; i < e; ++i) {
			value = (value << bits_per_byte) + (*i & byte_mask);
		}
		return value;
	}

	template<typename T, T& B, T& E, Error::Code C>
	int Const_Ptr<T, B, E, C>::get_int() const {
		check(int_size);
		if (*ptr_ != int_type) { err(Error::err_no_integer); }
		return get_int_value(ptr_ + 1);
	}

	signed char* ram_begin_;
	signed char* heap_end_;
	signed char* stack_begin_;
	signed char* ram_end_;

	template<typename T, T& B, T& E, Error::Code C>
	Value Const_Ptr<T, B, E, C>::get_value() {
		check(1); switch (*ptr_) {
			case ch_type: check(ch_size); return Value { ptr_[1] };
			case int_type:
				check(int_size); return Value { get_int_value(ptr_ + 1) };

			case ptr_type: {
				check(ptr_size);
				int offset { get_int_value(ptr_ + 1) };
				return Value { offset >= 0 ? ram_begin_ + offset : nullptr };
			}
			default: err(Error::err_unknown_type);
		}
	}

	template<typename T, T& B, T& E, Error::Code C>
	Const_Ptr<T, B, E, C> operator+(
		const Const_Ptr<T, B, E, C>& ptr, int offset
	) {
		return Const_Ptr<T, B, E, C> { ptr.get_raw() + offset };
	}

	template<typename T, T& B, T& E, Error::Code C>
	Const_Ptr<T, B, E, C> operator-(
		const Const_Ptr<T, B, E, C>& ptr, int offset
	) {
		return Const_Ptr<T, B, E, C> { ptr.get_raw() - offset };
	}

	template<typename T, T& B, T& E, Error::Code C>
	bool operator==(
		const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
	) {
		return a.get_raw() == b.get_raw();
	}

	template<typename T, T& B, T& E, Error::Code C>
	bool operator<(
		const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
	) {
		return a.get_raw() < b.get_raw();
	}

	using Code_Ptr = Const_Ptr<
	    const signed char*, code_begin_, code_end_,
		Error::err_leave_code_segment
	>;

	Code_Ptr pc_;

	template<signed char*& B, signed char*& E, Error::Code C>
	class Ptr : public Const_Ptr<signed char*, B, E, C> {
		public:
			explicit Ptr(signed char* ptr = nullptr):
				Const_Ptr<signed char*, B, E, C> { ptr } { }

			void set_ch(signed char value);
			void set_int(int value);
			void set_value(Value value);
	};

	template<signed char*& B, signed char*& E, Error::Code C>
	void Ptr<B, E, C>::set_ch(signed char value) {
		this->check(ch_size);
		this->ptr_[0] = ch_type; this->ptr_[1] = value;
	}

	void set_int_value(signed char* ptr, int value) {
		for (auto i { ptr + raw_int_size - 1 }; i >= ptr; --i) {
			*i = static_cast<signed char>(value);
			value >>= bits_per_byte;
		}
		if (value != 0 && value != -1) { err(Error::err_int_overflow); }
	}

	template<signed char*& B, signed char*& E, Error::Code C>
	void Ptr<B, E, C>::set_int(int value) {
		this->check(int_size);
		this->ptr_[0] = int_type;
		set_int_value(this->ptr_ + 1, value);
	}

	template<signed char*& B, signed char*& E, Error::Code C>
	void Ptr<B, E, C>::set_value(Value value) {
		if (const signed char* ch = std::get_if<signed char>(&value)) {
			this->check(ch_size);
			this->ptr_[0] = ch_type; this->ptr_[1] = *ch;
		} else if (int* val = std::get_if<int>(&value)) {
			this->check(int_size);
			this->ptr_[0] = int_type; set_int_value(this->ptr_ + 1, *val);
		} else if (auto ptr = std::get_if<signed char*>(&value)) {
			this->check(ptr_size);
			this->ptr_[0] = ptr_type;
			int v = *ptr ? static_cast<int>(*ptr - ram_begin_) : -1;
			set_int_value(this->ptr_ + 1, v);
		} else { err(Error::err_unknown_type); }
	}

	template<signed char*& B, signed char*& E, Error::Code C>
	Ptr<B, E, C> operator+(const Ptr<B, E, C>& ptr, int offset) {
		return Ptr<B, E, C>(ptr.get_raw() + offset);
	}

	template<signed char*& B, signed char*& E, Error::Code C>
	Ptr<B, E, C> operator-(const Ptr<B, E, C>& ptr, int offset) {
		return Ptr<B, E, C>(ptr.get_raw() - offset);
	}

	using Ram_Ptr = Ptr<ram_begin_, ram_end_, Error::err_leave_ram_segment>;

	template<signed char*& B, signed char*& E, Error::Code C>
	class Castable_Ptr : public Ptr<B, E, C> {
		public:
			explicit Castable_Ptr(signed char* ptr = nullptr):
				Ptr<B, E, C> { ptr } { }

			explicit Castable_Ptr(const Ram_Ptr ptr):
				Castable_Ptr { ptr.get_raw() }
			{ }

			explicit operator Ram_Ptr() { return Ram_Ptr { this->get_raw() }; }
	};

	template<signed char*& B, signed char*& E, Error::Code C>
	Castable_Ptr<B, E, C> operator+(const Castable_Ptr<B, E, C>& ptr, int offset) {
		return Castable_Ptr<B, E, C>(ptr.get_raw() + offset);
	}

	template<signed char*& B, signed char*& E, Error::Code C>
	Castable_Ptr<B, E, C> operator-(
		const Castable_Ptr<B, E, C>& ptr, int offset
	) { return Castable_Ptr<B, E, C>(ptr.get_raw() - offset); }

	using Heap_Ptr = Castable_Ptr<
		ram_begin_, heap_end_, Error::err_leave_heap_segment
	>;

	using Stack_Ptr = Castable_Ptr<
		stack_begin_, ram_end_, Error::err_leave_stack_segment
	>;

	template<typename P> P get_ptr(P ptr) {
		int value { get_int_value(ptr.get_raw()) };
		return P { value >= 0 ? ram_begin_ + value : nullptr };
	}

	template<typename P, typename Q> void set_ptr(P ptr, Q value) {
		auto got { value.get_raw() };
		set_int_value(ptr.get_raw(), got ? static_cast<int>(got - ram_begin_) : -1);
	}

	constexpr int node_next_offset { int_size };
	constexpr int node_prev_offset { 2 * int_size };

	template<typename P>
	struct List {
		P begin { };
		P end { };

		void insert(P node, P next);
		void remove(P node);
	};

	template<typename P>
	void List<P>::insert(P node, P next) {
		P prev;
		if (next) { prev = get_ptr(next + node_prev_offset); } else { prev = end; }
		set_ptr(node + node_next_offset, next);
		set_ptr(node + node_prev_offset, prev);
		if (next) { set_ptr(next + node_prev_offset, node); } else { end = node; }
		if (prev) { set_ptr(prev + node_next_offset, node); } else { begin = node; }
	}

	template<typename P>
	void List<P>::remove(P node) {
		auto next { get_ptr(node + node_next_offset) };
		auto prev { get_ptr(node + node_prev_offset) };
		signed char* n { next.get_raw() };
		signed char* p { prev.get_raw() };

		if (next) { set_ptr(next + node_prev_offset, prev); } else { end = prev; }
		if (prev) { set_ptr(prev + node_next_offset, next); } else { begin = next; }
	}

	List<Heap_Ptr> free_list;
	List<Heap_Ptr> alloc_list;

	Value pull_value() {
		auto value { Stack_Ptr { stack_begin_ }.get_value() };
		stack_begin_ += value_size(value); return value;
	}

	void push_value(Value value) {
		auto size { value_size(value) };
		if (heap_end_ + size > stack_begin_) { err(Error::err_stack_overflow); }
		stack_begin_ -= size;
		auto ptr { Stack_Ptr { stack_begin_ } };
		ptr.set_value(value);
	}

	signed char pull_ch() {
		auto value { Stack_Ptr { stack_begin_ }.get_ch() };
		stack_begin_ += ch_size; return value;
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

	void fetch_int(int offset) {
		push_int(Stack_Ptr { stack_begin_ + offset }.get_int());
	}

	void store_int(int offset) {
		auto value { pull_int() };
		Stack_Ptr { stack_begin_ + offset }.set_int(value);
	}

	Heap_Ptr pull_ptr() {
		if (stack_begin_ + ptr_size > ram_end_) {
			err(Error::err_leave_stack_segment);
		}
		Stack_Ptr node { stack_begin_ };
		if (*node.get_raw() != ptr_type) { err(Error::err_no_pointer); }
		int value { get_int_value((node + 1).get_raw()) };
		stack_begin_ += ptr_size;
		return Heap_Ptr { ram_begin_ + value };
	}

	void push_ptr(Heap_Ptr value) {
		if (heap_end_ + ptr_size > stack_begin_) {
			err(Error::err_stack_overflow);
		}
		stack_begin_ -= ptr_size; Stack_Ptr ptr { stack_begin_ };
		*ptr.get_raw() = ptr_type;
		set_ptr(ptr + 1, value);
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

	Heap_Ptr find_on_free_list(int size, bool tight_fit) {
		auto current { free_list.end };
		while (current) {
			int cur_size { get_int_value(current.get_raw()) };
			bool found {
				tight_fit ?
					cur_size == size || cur_size > 3 * size :
					cur_size >= size
			};
			if (found) {
				int rest_size { cur_size - size };
				if (rest_size >= heap_overhead) {
					Heap_Ptr rest_block { current + size };
					set_int_value(rest_block.get_raw(), rest_size);
					set_int_value(current.get_raw(), size);
					free_list.insert(rest_block, current);
				}
				free_list.remove(current);
				return current;
			}
			current = get_ptr(current + node_prev_offset);
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
			if (heap_end_ + size > stack_begin_) {
				err(Error::err_heap_overflow);
			}
			found = Heap_Ptr { heap_end_ };
			heap_end_ += size;
			set_int_value(found.get_raw(), size);
		}
		alloc_list.insert(found, alloc_list.begin);
		push_ptr(found + heap_overhead);
	}

	inline void insert_into_free_list(Heap_Ptr block) {
		Heap_Ptr greater { };
		Heap_Ptr smaller { free_list.begin };
		while (smaller && block < smaller) {
			greater = smaller;
			smaller = get_ptr(smaller + node_next_offset);
		}
		int size { get_int_value(block.get_raw()) };

		if (smaller) {
			int smaller_size { get_int_value(smaller.get_raw()) };
			if (smaller + smaller_size == block) {
				set_int_value(smaller.get_raw(), smaller_size + size);
				block = smaller; size += smaller_size;
				free_list.remove(block);
			}
		}

		if (greater) {
			if (block + size == greater) {
				set_int_value(block.get_raw(), size + get_int_value(greater.get_raw()));
				auto old_greater { greater };
				greater = get_ptr(greater + node_next_offset);
				free_list.remove(old_greater);
			}
		}

		if (block.get_raw() + size == heap_end_) {
			heap_end_ = block.get_raw();
		} else { free_list.insert(block, greater); }
	}

	void free_block(Heap_Ptr block) {
		block = block - heap_overhead;
		alloc_list.remove(block);
		int size { get_int_value(block.get_raw()) };
		if (size < std::max(node_size, heap_overhead)) {
			err(Error::err_free_invalid_block);
		}
		if (block.get_raw() + size > heap_end_) {
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
	free_list = List<Heap_Ptr> { };
	alloc_list = List<Heap_Ptr> { };
	pc_ = Code_Ptr { code_begin };
}

void vm::dump_stack() {
	std::cout << "STACK\n";
	for (auto i { stack_begin_}; i != ram_end_; ++i) {
		std::cout << "\t" << (int) *i << "\n";
	}
}

void vm::step() {
	signed char op { pc_.get_byte() }; pc_ = pc_ + 1;
	switch (op) {
		#if CONFIG_HAS_OP_NOP
			case op_nop: break;
		#endif
		#if CONFIG_HAS_OP_BREAK
			case op_break: err(Error::err_break);
		#endif
		case op_small_jmp: {
			auto value { pc_.get_byte() }; pc_ = pc_ + 1;
			jump(value, true_lit); break;
		}
		case op_small_jmp_false: {
			auto value { pc_.get_byte() }; pc_ = pc_ + 1;
			jump_with_stack_condition(value, true); break;
		}
		case op_small_jmp_true: {
			auto value { pc_.get_byte() }; pc_ = pc_ + 1;
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
			free_block(pull_ptr()); break;

		case op_pull:
			switch (Stack_Ptr { stack_begin_ }.get_byte()) {
				case ch_type: pull_ch(); break;
				case int_type: pull_int(); break;
				case ptr_type: pull_ptr(); break;
				default: err(Error::err_pull_unknown_type);
			} break;

		case op_dup: {
			switch (Stack_Ptr { stack_begin_ }.get_byte()) {
				case ch_type: {
					auto value { pull_ch() };
					push_ch(value); push_ch(value); break;
				}
				case int_type: {
					auto value { pull_int() };
					push_int(value); push_int(value); break;
				}
				case ptr_type: {
					auto value { pull_ptr() };
					push_ptr(value); push_ptr(value); break;
				}
				default: err(Error::err_dup_unknown_type);
			} break;
		}

		case op_swap: {
			auto a { pull_value() }; auto b { pull_value() };
			push_value(a); push_value(b);
			break;
		}

		#if CONFIG_HAS_CH
			case op_push_ch:
				push_ch(pc_.get_byte()); pc_ = pc_ + 1; break;

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
const signed char* vm::pc() { return pc_.get_raw(); }
