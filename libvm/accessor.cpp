#include <cstring>

#include "accessor.h"

using namespace vm;

#if CONFIG_WITH_INT
	template<typename T, T& B, T& E, Err::Code C>
	int Acc::get_int(const Const_Ptr<T, B, E, C>& ptr) {
		int value { 0 };
		for (
			auto i { ptr.ptr_ }, e { ptr.ptr_ + Int::raw_size };
			i < e; ++i
		) { value = (value << bits_per_byte) + (*i & byte_mask); }
		return value;
	}

	template<signed char*& B, signed char*& E, Err::Code C>
	void Acc::set_int(Ptr<B, E, C> ptr, int value) {
		for (
			auto i { ptr.ptr_ + Int::raw_size - 1 }; i >= ptr.ptr_; --i
		) {
			*i = static_cast<signed char>(value);
			value >>= bits_per_byte;
		}
		if (value != 0 && value != -1) { err(Err::int_overflow); }
	}
#endif

template<typename P> signed char Acc::get_byte(const P& ptr) {
	ptr.check(1); return *ptr.ptr_;
}

#if CONFIG_WITH_CALL
	static constexpr int stack_frame_pc { 1 };
	static constexpr int stack_frame_end { 1 + Int::raw_size };
	static constexpr int stack_frame_outer { 1 + 2 * Int::raw_size };
#endif

template<typename P> Value Acc::get_value(const P& ptr) {
	ptr.check(1); switch (*ptr.ptr_) {
		#if CONFIG_WITH_CHAR
			case Char::type_ch:
				ptr.check(Char::typed_size); return Value { ptr.ptr_[1] };
		#endif
		#if CONFIG_WITH_INT
			case Int::type_ch:
				ptr.check(Int::typed_size);
				return Value { get_int(ptr + 1) };
		#endif
		#if CONFIG_WITH_HEAP
			case ptr_type: {
				ptr.check(ptr_size);
				int offset { get_int(ptr + 1) };
				return Value { Heap_Ptr {
					offset >= 0 ? ram_begin + offset : nullptr
				} };
			}
		#endif
		#if CONFIG_WITH_CALL
			case stack_frame_type: {
				ptr.check(stack_frame_size);
				Stack_Frame frame;
				frame.pc = Code_Ptr { Code_Ptr::begin + get_int(ptr + stack_frame_pc) };
				frame.parent = Ram_Ptr { ram_begin + get_int(ptr + stack_frame_end) };
				frame.outer = Ram_Ptr { ram_begin + get_int(ptr + stack_frame_outer) };
				return frame;
			}
		#endif
		default: err(Err::unknown_type);
	}
}

static void set_ptr_type(signed char* ptr, signed char type) {
	if (*ptr && *ptr != type) { err(Err::change_type); }
	*ptr = type;
}

template<typename P>
void Acc::set_value(P ptr, const Value& value) {
	#if CONFIG_WITH_CHAR
		if (auto ch = std::get_if<signed char>(&value)) {
			ptr.check(Char::typed_size); set_ptr_type(ptr.ptr_, Char::type_ch);
			ptr.ptr_[1] = *ch;
			return;
		}
	#endif
	#if CONFIG_WITH_INT
		if (auto val = std::get_if<int>(&value)) {
			ptr.check(Int::typed_size); set_ptr_type(ptr.ptr_, Int::type_ch);
			set_int(ptr + 1, *val);
			return;
		}
	#endif
	#if CONFIG_WITH_HEAP
		if (auto pt = std::get_if<Heap_Ptr>(&value)) {
			ptr.check(ptr_size);
			set_ptr_type(ptr.ptr_, ptr_type);
			int v = *pt ? static_cast<int>(pt->ptr_ - ram_begin) : -1;
			set_int(ptr + 1, v);
			return;
		}
	#endif
	#if CONFIG_WITH_CALL
		if (auto sf = std::get_if<Stack_Frame>(&value)) {
			ptr.check(stack_frame_size);
			set_ptr_type(ptr.ptr_, stack_frame_type);
			set_int(ptr + stack_frame_pc, sf->pc.offset());
			set_int(ptr + stack_frame_end, sf->parent.offset());
			set_int(ptr + stack_frame_outer, sf->outer.offset());
			return;
		}
	#endif
	err(Err::unknown_type);
}

#if CONFIG_WITH_HEAP
	template<typename P> Heap_Ptr Acc::get_ptr(const P& ptr) {
		if (!ptr) { err(Err::null_access); }
		int value { get_int(ptr) };
		return Heap_Ptr { value >= 0 ? ram_begin + value : nullptr };
	}

	template<typename P> void Acc::set_ptr(P ptr, const Heap_Ptr& value) {
		auto got { value.ptr_ };
		if (!ptr) { err(Err::null_access); }
		set_int(ptr, got ? static_cast<int>(got - ram_begin) : -1);
	}
#endif

Value Acc::pull() {
	auto value { get_value(Stack_Ptr { stack_begin }) };
	stack_begin += value_size(value); return value;
}

#if CONFIG_WITH_CHAR
	signed char Acc::pull_ch() {
		auto value { pull() };
		auto ch = std::get_if<signed char>(&value);
		if (!ch) { err(Err::no_char); }
		return *ch;
	}
#endif

#if CONFIG_WITH_INT
	int Acc::pull_int() { return int_value(pull()); }
#endif

#if CONFIG_WITH_HEAP
	Heap_Ptr Acc::pull_ptr() {
		auto value { pull() };
		auto ptr = std::get_if<Heap_Ptr>(&value);
		if (!ptr) { err(Err::no_pointer); }
		return *ptr;
	}
#endif

Stack_Ptr Acc::push(Value value, int after_values) {
	auto size { value_size(value) };
	if (stack_lower_limit() + size > stack_begin) { err(Err::stack_overflow); }
	Stack_Ptr position { stack_begin };
	if (after_values) {
		for (; after_values; --after_values) {
			position = position + value_size(*position.ptr_);
		}
		memmove(stack_begin - size, stack_begin, position.ptr_ - stack_begin);
	}
	stack_begin -= size;
	position = position - size;
	std::memset(position.ptr_, 0, size);
	set_value(position, value);
	return position;
}

// instantiate templates:

#if CONFIG_WITH_INT
	template int Acc::get_int(
		const Const_Ptr<
			const signed char*, old_code_begin, old_code_end, Err::leave_code_segment
		>& ptr
	);
#endif

#if CONFIG_WITH_HEAP
	template Value Acc::get_value(const Heap_Ptr&);
#endif

template Value Acc::get_value(const Ram_Ptr&);

template Value Acc::get_value(const Stack_Ptr&);

#if CONFIG_WITH_HEAP
	template void Acc::set_value(
		Ptr<ram_begin, heap_end, Err::leave_heap_segment> ptr, const Value& value
	);

	template Heap_Ptr Acc::get_ptr(const Heap_Ptr&);
	template void Acc::set_ptr(Heap_Ptr, const Heap_Ptr&);
#endif
