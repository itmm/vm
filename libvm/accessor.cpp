#include "accessor.h"

using namespace vm;

template<typename T, T& B, T& E, Err::Code C>
int Acc::get_int_value(const Const_Ptr<T, B, E, C>& ptr) {
	int value { 0 };
	for (
		auto i { ptr.ptr_ }, e { ptr.ptr_ + raw_int_size };
		i < e; ++i
		) { value = (value << bits_per_byte) + (*i & byte_mask); }
	return value;
}

template<signed char*& B, signed char*& E, Err::Code C>
void Acc::set_int(Ptr<B, E, C> ptr, int value) {
	for (
		auto i { ptr.ptr_ + raw_int_size - 1 }; i >= ptr.ptr_; --i
		) {
		*i = static_cast<signed char>(value);
		value >>= bits_per_byte;
	}
	if (value != 0 && value != -1) { err(Err::int_overflow); }
}

template<typename T, T& B, T& E, Err::Code C>
signed char Acc::get_byte(const Const_Ptr<T, B, E, C>& ptr) {
	ptr.check(1); return *ptr.ptr_;
}

template<typename T, T& B, T& E, Err::Code C>
Value Acc::get_value(const Const_Ptr<T, B, E, C>& ptr) {
	ptr.check(1); switch (*ptr.ptr_) {
		case ch_type:
			ptr.check(ch_size); return Value { ptr.ptr_[1] };

		case int_type:
			ptr.check(int_size);
			return Value { get_int_value(ptr + 1) };

		case ptr_type: {
			ptr.check(ptr_size);
			int offset { get_int_value(ptr + 1) };
			return Value { Heap_Ptr {
				offset >= 0 ? ram_begin + offset : nullptr
			} };
		}
		default: err(Err::unknown_type);
	}
}

template<signed char*& B, signed char*& E, Err::Code C>
void Acc::set_value(Ptr<B, E, C> ptr, const Value& value) {
	if (auto ch = std::get_if<signed char>(&value)) {
		ptr.check(ch_size);
		if (ptr.ptr_[0] && ptr.ptr_[0] != ch_type) { err(Err::change_type); }
		ptr.ptr_[0] = ch_type; ptr.ptr_[1] = *ch;
	} else if (auto val = std::get_if<int>(&value)) {
		ptr.check(int_size);
		if (ptr.ptr_[0] && ptr.ptr_[0] != int_type) { err(Err::change_type); }
		ptr.ptr_[0] = int_type;
		set_int(ptr + 1, *val);
	} else if (auto pt = std::get_if<Heap_Ptr>(&value)) {
		ptr.check(ptr_size);
		if (ptr.ptr_[0] && ptr.ptr_[0] != ptr_type) { err(Err::change_type); }
		ptr.ptr_[0] = ptr_type;
		int v = *pt ? static_cast<int>(pt->ptr_ - ram_begin) : -1;
		set_int(ptr + 1, v);
	} else { err(Err::unknown_type); }
}

template<typename P> Heap_Ptr Acc::get_ptr(const P& ptr) {
	if (!ptr) { err(Err::null_access); }
	int value { get_int_value(ptr) };
	return Heap_Ptr { value >= 0 ? ram_begin + value : nullptr };
}

template<typename P> void Acc::set_ptr(P ptr, const Heap_Ptr& value) {
	auto got { value.ptr_ };
	if (!ptr) { err(Err::null_access); }
	set_int(ptr, got ? static_cast<int>(got - ram_begin) : -1);
}

Value Acc::pull() {
	auto value { get_value(Stack_Ptr { stack_begin }) };
	stack_begin += value_size(value); return value;
}

signed char Acc::pull_ch() {
	auto value { pull() };
	auto ch = std::get_if<signed char>(&value);
	if (!ch) { err(Err::no_char); }
	return *ch;
}

int Acc::pull_int() { return int_value(pull()); }


Heap_Ptr Acc::pull_ptr() {
	auto value { pull() };
	auto ptr = std::get_if<Heap_Ptr>(&value);
	if (!ptr) { err(Err::no_pointer); }
	return *ptr;
}

void Acc::push(Value value) {
	auto size { value_size(value) };
	if (heap_end + size > stack_begin) { err(Err::stack_overflow); }
	stack_begin -= size;
	std::memset(stack_begin, 0, size);
	auto ptr { Stack_Ptr { stack_begin } };
	set_value(ptr, value);
}

// instantiate templates:

template int Acc::get_int_value(
	const Const_Ptr<
	    const signed char*, code_begin, code_end, Err::leave_code_segment
	>& ptr
);

template signed char Acc::get_byte(
	const Const_Ptr<
	    const signed char*, code_begin, code_end, Err::leave_code_segment
	>& ptr
);

template Value Acc::get_value(
	const Const_Ptr<
	    signed char*, ram_begin, heap_end, Err::leave_heap_segment
	>& ptr
);

template void Acc::set_value(
	Ptr<ram_begin, heap_end, Err::leave_heap_segment> ptr, const Value& value
);

template Heap_Ptr Acc::get_ptr(
	const Casting_Ptr<ram_begin, heap_end, Err::leave_heap_segment>& ptr
);

template void Acc::set_ptr(
	Casting_Ptr<ram_begin, heap_end, Err::leave_heap_segment> ptr,
	const Heap_Ptr& value
);
