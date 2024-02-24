#include <cstring>

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

static constexpr int stack_frame_pc { 1 };
static constexpr int stack_frame_end { 1 + raw_int_size };
static constexpr int stack_frame_outer { 1 + 2 * raw_int_size };

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

		case stack_frame_type: {
			ptr.check(stack_frame_size);
			Stack_Frame frame;
			frame.pc = Code_Ptr { code_begin + get_int_value(ptr + stack_frame_pc) };
			frame.parent = Ram_Ptr { ram_begin + get_int_value(ptr + stack_frame_end) };
			frame.outer = Ram_Ptr { ram_begin + get_int_value(ptr + stack_frame_outer) };
			return frame;
		}
		default: err(Err::unknown_type);
	}
}

static void set_ptr_type(signed char* ptr, signed char type) {
	if (*ptr && *ptr != type) { err(Err::change_type); }
	*ptr = type;
}

template<signed char*& B, signed char*& E, Err::Code C>
void Acc::set_value(Ptr<B, E, C> ptr, const Value& value) {
	if (auto ch = std::get_if<signed char>(&value)) {
		ptr.check(ch_size); set_ptr_type(ptr.ptr_, ch_type);
		ptr.ptr_[1] = *ch;
	} else if (auto val = std::get_if<int>(&value)) {
		ptr.check(int_size); set_ptr_type(ptr.ptr_, int_type);
		set_int(ptr + 1, *val);
	} else if (auto pt = std::get_if<Heap_Ptr>(&value)) {
		ptr.check(ptr_size);
		set_ptr_type(ptr.ptr_, ptr_type);
		int v = *pt ? static_cast<int>(pt->ptr_ - ram_begin) : -1;
		set_int(ptr + 1, v);
	} else if (auto sf = std::get_if<Stack_Frame>(&value)) {
		ptr.check(stack_frame_size);
		set_ptr_type(ptr.ptr_, stack_frame_type);
		set_int(ptr + stack_frame_pc, sf->pc.offset());
		set_int(ptr + stack_frame_end, sf->parent.offset());
		set_int(ptr + stack_frame_outer, sf->outer.offset());
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
		signed char*, ram_begin, ram_end, Err::leave_ram_segment
	>& ptr
);

template Value Acc::get_value(
	const Const_Ptr<
		signed char*, ram_begin, heap_end, Err::leave_heap_segment
	>& ptr
);

template Value Acc::get_value(
	const Const_Ptr<
	    signed char*, stack_begin, stack_end, Err::leave_stack_segment
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
