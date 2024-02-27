#include "ptr.h"

using namespace vm;

const signed char* vm::old_code_begin;
const signed char* vm::old_code_end;
signed char* vm::ram_begin;
#if CONFIG_WITH_HEAP
	signed char* vm::heap_end;
#endif
signed char* vm::stack_begin;
#if CONFIG_WITH_CALL
	signed char* vm::stack_end;
#endif
signed char* vm::ram_end;

vm::Code_Ptr vm::pc;

template<typename T, T& B, T& E, Err::Code C>
void Const_Ptr<T, B, E, C>::internal_check(int size, T begin, T end, Err::Code code) const {
	if (ptr_ < begin || ptr_ + size > end) { err(code); }
}

template<typename P> P vm::operator+(const P& ptr, int offset) {
	return P { ptr.ptr_ + offset };
}

template<typename P> P vm::operator-(const P& ptr, int offset) {
	return P { ptr.ptr_ - offset };
}

template<typename T, T& B, T& E, Err::Code C>
bool vm::operator==(
	const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
) { return a.ptr_ == b.ptr_; }

template<typename T, T& B, T& E, Err::Code C>
bool vm::operator<(
	const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
) { return a.ptr_ < b.ptr_; }

const signed char* Code_Ptr::begin { nullptr };
const signed char* Code_Ptr::end { nullptr };

signed char* Ram_Ptr::begin { nullptr };
signed char* Ram_Ptr::end { nullptr };

#if CONFIG_WITH_HEAP
	signed char* Heap_Ptr::begin { nullptr };
	signed char* Heap_Ptr::end { nullptr };
#endif

signed char* Stack_Ptr::begin { nullptr };
signed char* Stack_Ptr::end { nullptr };

// instantiate templates:

template class vm::Const_Ptr<const signed char*, old_code_begin, old_code_end, Err::leave_code_segment>;

template void vm::Const_Ptr<signed char*, ram_begin, ram_end, Err::leave_ram_segment>::internal_check(int, signed char*, signed char*, Err::Code) const;
#if CONFIG_WITH_HEAP
	template void vm::Const_Ptr<signed char*, ram_begin, heap_end, Err::leave_heap_segment>::internal_check(int, signed char*, signed char*, Err::Code) const;
#endif
#if CONFIG_WITH_CALL
	template void vm::Const_Ptr<signed char*, stack_begin, stack_end, Err::leave_stack_segment>::internal_check(int, signed char*, signed char*, Err::Code) const;
#else
	template void vm::Const_Ptr<signed char*, stack_begin, ram_end, Err::leave_stack_segment>::check(int) const;
	template void vm::Const_Ptr<signed char*, stack_begin, ram_end, Err::leave_stack_segment>::internal_check(int, signed char*, signed char*, Err::Code) const;
#endif

template Code_Ptr vm::operator+(const Code_Ptr&, int);
template Const_Ptr<const signed char*, old_code_begin, old_code_end, Err::leave_code_segment> vm::operator+(
	const Const_Ptr<const signed char*, old_code_begin, old_code_end, Err::leave_code_segment>&, int);
template Ram_Ptr vm::operator+(const Ram_Ptr&, int);
template Const_Ptr<signed char*, ram_begin, ram_end, Err::leave_ram_segment> vm::operator+(
	const Const_Ptr<signed char*, ram_begin, ram_end, Err::leave_ram_segment>&, int);
template Ptr<ram_begin, ram_end, Err::leave_ram_segment> vm::operator+(
	const Ptr<ram_begin, ram_end, Err::leave_ram_segment>&, int);

#if CONFIG_WITH_HEAP
	template Heap_Ptr vm::operator+(const Heap_Ptr&, int);
	template Ptr<ram_begin, heap_end, Err::leave_heap_segment> vm::operator+(
		const Ptr<ram_begin, heap_end, Err::leave_heap_segment>&, int);
#endif

template Stack_Ptr vm::operator+(const Stack_Ptr&, int);
template Stack_Ptr vm::operator-(const Stack_Ptr&, int);

#if CONFIG_WITH_CALL
	template Const_Ptr<signed char*, stack_begin, stack_end, Err::leave_stack_segment>
	vm::operator+(
		const Const_Ptr<
			signed char*, stack_begin, stack_end, Err::leave_stack_segment
		>&, int
	);

	template Ptr<stack_begin, stack_end, Err::leave_stack_segment>
	vm::operator+(const Ptr<stack_begin, stack_end, Err::leave_stack_segment>&, int);

	template Casting_Ptr<stack_begin, stack_end, Err::leave_stack_segment>
	vm::operator+(
		const Casting_Ptr<stack_begin, stack_end, Err::leave_stack_segment>&, int
	);
#else
	template Const_Ptr<signed char*, stack_begin, ram_end, Err::leave_stack_segment>
	vm::operator+(
		const Const_Ptr<
			signed char*, stack_begin, ram_end, Err::leave_stack_segment
		>&, int
	);

	template Ptr<stack_begin, ram_end, Err::leave_stack_segment>
	vm::operator+(const Ptr<stack_begin, ram_end, Err::leave_stack_segment>&, int);

	template Casting_Ptr<stack_begin, ram_end, Err::leave_stack_segment>
	vm::operator+(
		const Casting_Ptr<stack_begin, ram_end, Err::leave_stack_segment>&, int
	);
#endif

#if CONFIG_WITH_HEAP
	template Heap_Ptr vm::operator-(const Heap_Ptr&, int);
#endif

#if CONFIG_WITH_CALL
	template Casting_Ptr<stack_begin, stack_end, Err::leave_stack_segment>
	vm::operator-(
		const Casting_Ptr<stack_begin, stack_end, Err::leave_stack_segment>&, int
	);
#else
	template Casting_Ptr<stack_begin, ram_end, Err::leave_stack_segment>
	vm::operator-(
		const Casting_Ptr<stack_begin, ram_end, Err::leave_stack_segment>&, int
	);
#endif

template bool vm::operator==(
	const Const_Ptr<
		const signed char*, old_code_begin, old_code_end, Err::leave_code_segment
	>&,
	const Const_Ptr<const signed char*, old_code_begin, old_code_end, Err::leave_code_segment>&
);

template bool vm::operator==(
	const Const_Ptr<
		signed char*, ram_begin, ram_end, Err::leave_ram_segment
	>&,
	const Const_Ptr<signed char*, ram_begin, ram_end, Err::leave_ram_segment>&
);

#if CONFIG_WITH_HEAP
	template bool vm::operator==(
		const Const_Ptr<
			signed char*, ram_begin, heap_end, Err::leave_heap_segment
		>&,
		const Const_Ptr<signed char*, ram_begin, heap_end, Err::leave_heap_segment>&
	);
#endif

#if CONFIG_WITH_CALL
	template bool vm::operator==(
		const Const_Ptr<
			signed char*, stack_begin, stack_end, Err::leave_stack_segment
		>&,
		const Const_Ptr<
			signed char*, stack_begin, stack_end, Err::leave_stack_segment
		>&
	);
#else
	template bool vm::operator==(
		const Const_Ptr<
			signed char*, stack_begin, ram_end, Err::leave_stack_segment
		>&,
		const Const_Ptr<
			signed char*, stack_begin, ram_end, Err::leave_stack_segment
		>&
	);
#endif

template bool vm::operator<(
	const Const_Ptr<
		const signed char*, old_code_begin, old_code_end, Err::leave_code_segment
	>&,
	const Const_Ptr<const signed char*, old_code_begin, old_code_end, Err::leave_code_segment>&
);

template bool vm::operator<(
	const Const_Ptr<
		signed char*, ram_begin, ram_end, Err::leave_ram_segment
	>&,
	const Const_Ptr<signed char*, ram_begin, ram_end, Err::leave_ram_segment>&
);

#if CONFIG_WITH_HEAP
	template bool vm::operator<(
		const Const_Ptr<
			signed char*, ram_begin, heap_end, Err::leave_heap_segment
		>&,
		const Const_Ptr<signed char*, ram_begin, heap_end, Err::leave_heap_segment>&
	);
#endif

#if CONFIG_WITH_CALL
	template bool vm::operator<(
		const Const_Ptr<
			signed char*, stack_begin, stack_end, Err::leave_stack_segment
		>&,
		const Const_Ptr<
			signed char*, stack_begin, stack_end, Err::leave_stack_segment
		>&
	);
#else
	template bool vm::operator<(
		const Const_Ptr<
			signed char*, stack_begin, ram_end, Err::leave_stack_segment
		>&,
		const Const_Ptr<
			signed char*, stack_begin, ram_end, Err::leave_stack_segment
		>&
	);
#endif

template class vm::Ptr<ram_begin, ram_end, Err::leave_ram_segment>;

#if CONFIG_WITH_HEAP
	template class vm::Casting_Ptr<ram_begin, heap_end, Err::leave_heap_segment>;
#endif

#if CONFIG_WITH_CALL
	template class vm::Casting_Ptr<
		stack_begin, stack_end, Err::leave_stack_segment
	>;
#else
	template class vm::Casting_Ptr<
		stack_begin, ram_end, Err::leave_stack_segment
	>;
#endif
