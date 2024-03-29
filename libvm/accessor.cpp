#include <cstring>

#include "accessor.h"

using namespace vm;

#if CONFIG_WITH_INT
	template<typename P> Int Acc::get_int(const P& ptr) {
		Int::value_type value { 0 };
		for (
			auto i { ptr.ptr_ }, e { ptr.ptr_ + Int::raw_size };
			i < e; ++i
		) { value = (value << bits_per_byte) + (*i & byte_mask); }
		return Int { value };
	}

	template<typename P> void Acc::set_int(P ptr, const Int& value) {
		auto v { value.value };
		for (
			auto i { ptr.ptr_ + Int::raw_size - 1 }; i >= ptr.ptr_; --i
		) {
			*i = static_cast<signed char>(v);
			v >>= bits_per_byte;
		}
		if (v != 0 && v != -1) { err(Err::int_overflow); }
	}
#endif

template<typename P> signed char Acc::get_byte(const P& ptr) {
	ptr.check(1); return *ptr.ptr_;
}

#if CONFIG_WITH_CALL
	static constexpr int stack_frame_pc { 1 };
	static constexpr int stack_frame_end { 1 + 1 * Int::raw_size };
	static constexpr int stack_frame_outer { 1 + 2 * Int::raw_size };
	static_assert(stack_frame_outer + Int::raw_size <= Stack_Frame::typed_size);
	#if CONFIG_WITH_EXCEPTIONS
		static constexpr int stack_frame_catch_pc { 1 + 3 * Int::raw_size };
		static_assert(
			stack_frame_catch_pc + Int::raw_size <= Stack_Frame::typed_size
		);
	#endif

	template<typename P> Code_Ptr get_code_ptr(const P& ptr) {
		auto value { Acc::get_int(ptr) };
		return Code_Ptr { value.value >= 0 ? Code_Ptr::begin + value : nullptr };
	}

	template<typename P> Stack_Ptr get_stack_ptr(const P& ptr) {
		auto value { Acc::get_int(ptr) };
		return Stack_Ptr { value.value >= 0 ? Stack_Ptr::begin + value : nullptr };
	}
#endif

template<typename P> Value Acc::get_value(const P& ptr) {
	ptr.check(1); switch (*ptr.ptr_) {
		#if CONFIG_WITH_BYTE
			case Byte::type_ch:
				ptr.check(Byte::typed_size);
				return Value { Byte { ptr.ptr_[1] } };
		#endif
		#if CONFIG_WITH_INT
			case Int::type_ch:
				ptr.check(Int::typed_size);
				return Value { Int { get_int(ptr + 1) } };
		#endif
		#if CONFIG_WITH_HEAP
			case ptr_type: {
				ptr.check(ptr_size);
				auto offset { get_int(ptr + 1).value };
				return Value { Heap_Ptr {
					offset >= 0 ? Ram_Ptr::begin + offset : nullptr
				} };
			}
		#endif
		#if CONFIG_WITH_CALL
			case Stack_Frame::type_ch: {
				ptr.check(Stack_Frame::typed_size);
				Stack_Frame frame;
				frame.pc = get_code_ptr(ptr + stack_frame_pc);
				#if CONFIG_WITH_EXCEPTIONS
					frame.catch_pc = get_code_ptr(ptr + stack_frame_catch_pc);
				#endif
				frame.parent = get_stack_ptr(ptr + stack_frame_end);
				frame.outer = get_stack_ptr(ptr + stack_frame_outer);
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
	#if CONFIG_WITH_BYTE
		if (auto ch = std::get_if<Byte>(&value)) {
			ptr.check(Byte::typed_size); set_ptr_type(ptr.ptr_, Byte::type_ch);
			ptr.ptr_[1] = ch->value;
			return;
		}
	#endif
	#if CONFIG_WITH_INT
		if (auto val = std::get_if<Int>(&value)) {
			ptr.check(Int::typed_size); set_ptr_type(ptr.ptr_, Int::type_ch);
			set_int(ptr + 1, *val);
			return;
		}
	#endif
	#if CONFIG_WITH_HEAP
		if (auto pt = std::get_if<Heap_Ptr>(&value)) {
			ptr.check(ptr_size);
			set_ptr_type(ptr.ptr_, ptr_type);
			auto v = *pt ? static_cast<Int::value_type>(pt->ptr_ - Ram_Ptr::begin) : -1;
			set_int(ptr + 1, Int { v });
			return;
		}
	#endif
	#if CONFIG_WITH_CALL
		if (auto sf = std::get_if<Stack_Frame>(&value)) {
			ptr.check(Stack_Frame::typed_size);
			set_ptr_type(ptr.ptr_, Stack_Frame::type_ch);
			set_int(ptr + stack_frame_pc, Int { sf->pc.offset() });
			#if CONFIG_WITH_EXCEPTIONS
			set_int(ptr + stack_frame_catch_pc, Int { sf->catch_pc.offset() });
			#endif
			set_int(ptr + stack_frame_end, Int { sf->parent.offset() });
			set_int(ptr + stack_frame_outer, Int { sf->outer.offset() });
			return;
		}
	#endif
	err(Err::unknown_type);
}

#if CONFIG_WITH_HEAP
	template<typename P> Heap_Ptr Acc::get_ptr(const P& ptr) {
		if (!ptr) { err(Err::null_access); }
		auto value { get_int(ptr) };
		return Heap_Ptr { value.value >= 0 ? Ram_Ptr::begin + value : nullptr };
	}

	template<typename P> void Acc::set_ptr(P ptr, const Heap_Ptr& value) {
		auto got { value.ptr_ };
		if (!ptr) { err(Err::null_access); }
		set_int(ptr, Int { got ? static_cast<Int::value_type>(got - Ram_Ptr::begin) : -1 });
	}
#endif

Value Acc::pull() {
	auto value { get_value(Stack_Ptr { Stack_Ptr::begin }) };
	Stack_Ptr::begin += value_size(value);
	return value;
}

#if CONFIG_WITH_BYTE
	Byte Acc::pull_ch() {
		auto value { pull() };
		auto ch = std::get_if<Byte>(&value);
		if (!ch) { err(Err::no_char); }
		return *ch;
	}
#endif

#if CONFIG_WITH_NUMERIC
	Int::internal_type Acc::pull_internal_int() {
		return internal_int_value(pull());
	}
#endif

#if CONFIG_WITH_INT
	#if ! CONFIG_INTERNAL_INT_IS_INT
		Int Acc::pull_int() { return int_value(pull()); }
	#endif
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
	if (stack_lower_limit() + size > Stack_Ptr::begin) { err(Err::stack_overflow); }
	Stack_Ptr position { Stack_Ptr::begin };
	if (after_values) {
		for (; after_values; --after_values) {
			position = position + value_size(*position.ptr_);
		}
		memmove(Stack_Ptr::begin - size, Stack_Ptr::begin, position.ptr_ - Stack_Ptr::begin);
	}
	Stack_Ptr::begin -= size;
	position = position - size;
	std::memset(position.ptr_, 0, size);
	set_value(position, value);
	return position;
}

// instantiate templates:

template signed char Acc::get_byte(const Code_Ptr&);

#if CONFIG_WITH_INT
	template Int Acc::get_int(const Code_Ptr&);
#endif

#if CONFIG_WITH_HEAP
	template Value Acc::get_value(const Heap_Ptr&);
#endif

template Value Acc::get_value(const Stack_Ptr&);

#if CONFIG_WITH_HEAP
	template void Acc::set_value(Heap_Ptr ptr, const Value& value);
	template Heap_Ptr Acc::get_ptr(const Heap_Ptr&);
	template void Acc::set_ptr(Heap_Ptr, const Heap_Ptr&);
#endif
