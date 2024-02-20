#include "ptr.h"

using namespace vm;

const signed char* vm::code_begin;
const signed char* vm::code_end;
signed char* vm::ram_begin;
signed char* vm::heap_end;
signed char* vm::stack_begin;
signed char* vm::ram_end;

vm::Code_Ptr vm::pc;

template<typename T, T& B, T& E, Err::Code C>
void Const_Ptr<T, B, E, C>::check(int size) const {
	if (ptr_ < B || ptr_ + size > E) { err(C); }
}

template<typename T, T& B, T& E, Err::Code C>
Const_Ptr<T, B, E, C> vm::operator+(const Const_Ptr<T, B, E, C>& ptr, int offset) {
	return Const_Ptr<T, B, E, C> { ptr.ptr_ + offset };
}

template<typename T, T& B, T& E, Err::Code C>
Const_Ptr<T, B, E, C> vm::operator-(const Const_Ptr<T, B, E, C>& ptr, int offset) {
	return Const_Ptr<T, B, E, C> { ptr.ptr_ - offset };
}

template<typename T, T& B, T& E, Err::Code C>
bool vm::operator==(
	const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
) { return a.ptr_ == b.ptr_; }

template<typename T, T& B, T& E, Err::Code C>
bool vm::operator<(
	const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
) { return a.ptr_ < b.ptr_; }

template class vm::Const_Ptr<
	const signed char*, code_begin, code_end,
	Err::leave_code_segment
>;

template Const_Ptr<const signed char*, code_begin, code_end, Err::leave_code_segment>
vm::operator+(
	const Const_Ptr<const signed char*, code_begin, code_end, Err::leave_code_segment>&,
	int
);

template Const_Ptr<signed char*, ram_begin, heap_end, Err::leave_heap_segment>
vm::operator+(
	const Const_Ptr<signed char*, ram_begin, heap_end, Err::leave_heap_segment>&,
	int
);

template Const_Ptr<signed char*, stack_begin, ram_end, Err::leave_stack_segment>
vm::operator+(
	const Const_Ptr<signed char*, stack_begin, ram_end, Err::leave_stack_segment>&,
	int
);

template bool vm::operator==(
	const Const_Ptr<signed char*, ram_begin, heap_end, Err::leave_heap_segment>&,
	const Const_Ptr<signed char*, ram_begin, heap_end, Err::leave_heap_segment>&
);

template bool vm::operator<(
	const Const_Ptr<signed char*, ram_begin, heap_end, Err::leave_heap_segment>&,
	const Const_Ptr<signed char*, ram_begin, heap_end, Err::leave_heap_segment>&
);

template class vm::Ptr<ram_begin, ram_end, Err::leave_ram_segment>;

template class vm::Casting_Ptr<ram_begin, heap_end, Err::leave_heap_segment>;

template class vm::Casting_Ptr<stack_begin, ram_end, Err::leave_stack_segment>;
