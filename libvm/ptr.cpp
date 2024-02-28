#include "ptr.h"

using namespace vm;

const signed char* vm::old_code_begin;
const signed char* vm::old_code_end;
signed char* vm::old_ram_begin;
#if CONFIG_WITH_HEAP
	signed char* vm::old_heap_end;
#endif
signed char* vm::old_stack_begin;
#if CONFIG_WITH_CALL
	signed char* vm::old_stack_end;
#endif
signed char* vm::old_ram_end;

vm::Code_Ptr vm::pc;

template<typename T, T& B, T& E>
void Const_Ptr<T, B, E>::internal_check(int size, const signed char* begin, const signed char* end, Err::Code code) const {
	if (ptr_ < begin || ptr_ + size > end) { err(code); }
}

template<typename T, T& B, T& E>
int Const_Ptr<T, B, E>::internal_offset(const signed char* begin) const {
	return ptr_ ? static_cast<int>(ptr_ - begin) : -1;
}

template<typename P> P vm::operator+(const P& ptr, int offset) {
	return P { ptr.ptr_ + offset };
}

template<typename P> P vm::operator-(const P& ptr, int offset) {
	return P { ptr.ptr_ - offset };
}

template<typename T, T& B, T& E>
bool vm::operator==(
	const Const_Ptr<T, B, E>& a, const Const_Ptr<T, B, E>& b
) { return a.ptr_ == b.ptr_; }

template<typename T, T& B, T& E>
bool vm::operator<(
	const Const_Ptr<T, B, E>& a, const Const_Ptr<T, B, E>& b
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
#if CONFIG_WITH_CALL
	signed char* Stack_Ptr::end { nullptr };
#endif

// instantiate templates:

template class vm::Const_Ptr<const signed char*, old_code_begin, old_code_end>;

template void vm::Const_Ptr<signed char*, old_ram_begin, old_ram_end>::internal_check(int, const signed char*, const signed char*, Err::Code) const;
template int vm::Const_Ptr<signed char*, old_ram_begin, old_ram_end>::internal_offset(const signed char*) const;
#if CONFIG_WITH_HEAP
	template void vm::Const_Ptr<signed char*, old_ram_begin, old_heap_end>::internal_check(int, const signed char*, const signed char*, Err::Code) const;
	template int vm::Const_Ptr<signed char*, old_ram_begin, old_heap_end>::internal_offset(const signed char*) const;
#endif
#if CONFIG_WITH_CALL
	template void vm::Const_Ptr<signed char*, old_stack_begin, old_stack_end>::internal_check(int, const signed char*, const signed char*, Err::Code) const;
	template int vm::Const_Ptr<signed char*, old_stack_begin, old_stack_end>::internal_offset(const signed char*) const;
#else
	template void vm::Const_Ptr<signed char*, old_stack_begin, old_ram_end>::internal_check(int, const signed char*, const signed char*, Err::Code) const;
	template int vm::Const_Ptr<signed char*, old_stack_begin, old_ram_end>::internal_offset(const signed char*) const;
#endif

template Code_Ptr vm::operator+(const Code_Ptr&, int);
template Const_Ptr<const signed char*, old_code_begin, old_code_end> vm::operator+(
	const Const_Ptr<const signed char*, old_code_begin, old_code_end>&, int);
template Ram_Ptr vm::operator+(const Ram_Ptr&, int);
template Const_Ptr<signed char*, old_ram_begin, old_ram_end> vm::operator+(
	const Const_Ptr<signed char*, old_ram_begin, old_ram_end>&, int);
template Ptr<old_ram_begin, old_ram_end> vm::operator+(
	const Ptr<old_ram_begin, old_ram_end>&, int);

#if CONFIG_WITH_HEAP
	template Heap_Ptr vm::operator+(const Heap_Ptr&, int);
	template Ptr<old_ram_begin, old_heap_end> vm::operator+(
		const Ptr<old_ram_begin, old_heap_end>&, int);
#endif

template Stack_Ptr vm::operator+(const Stack_Ptr&, int);
template Stack_Ptr vm::operator-(const Stack_Ptr&, int);

#if CONFIG_WITH_CALL
	template Const_Ptr<signed char*, old_stack_begin, old_stack_end>
	vm::operator+(
		const Const_Ptr<
			signed char*, old_stack_begin, old_stack_end
		>&, int
	);

	template Ptr<old_stack_begin, old_stack_end>
	vm::operator+(const Ptr<old_stack_begin, old_stack_end>&, int);

	template Casting_Ptr<old_stack_begin, old_stack_end>
	vm::operator+(
		const Casting_Ptr<old_stack_begin, old_stack_end>&, int
	);
#else
	template Const_Ptr<signed char*, old_stack_begin, old_ram_end>
	vm::operator+(
		const Const_Ptr<
			signed char*, old_stack_begin, old_ram_end
		>&, int
	);

	template Ptr<old_stack_begin, old_ram_end>
	vm::operator+(const Ptr<old_stack_begin, old_ram_end>&, int);

	template Casting_Ptr<old_stack_begin, old_ram_end>
	vm::operator+(
		const Casting_Ptr<old_stack_begin, old_ram_end>&, int
	);
#endif

#if CONFIG_WITH_HEAP
	template Heap_Ptr vm::operator-(const Heap_Ptr&, int);
#endif

#if CONFIG_WITH_CALL
	template Casting_Ptr<old_stack_begin, old_stack_end>
	vm::operator-(
		const Casting_Ptr<old_stack_begin, old_stack_end>&, int
	);
#else
	template Casting_Ptr<old_stack_begin, old_ram_end>
	vm::operator-(
		const Casting_Ptr<old_stack_begin, old_ram_end>&, int
	);
#endif

template bool vm::operator==(
	const Const_Ptr<
		const signed char*, old_code_begin, old_code_end
	>&,
	const Const_Ptr<const signed char*, old_code_begin, old_code_end>&
);

template bool vm::operator==(
	const Const_Ptr<
		signed char*, old_ram_begin, old_ram_end
	>&,
	const Const_Ptr<signed char*, old_ram_begin, old_ram_end>&
);

#if CONFIG_WITH_HEAP
	template bool vm::operator==(
		const Const_Ptr<
			signed char*, old_ram_begin, old_heap_end
		>&,
		const Const_Ptr<signed char*, old_ram_begin, old_heap_end>&
	);
#endif

#if CONFIG_WITH_CALL
	template bool vm::operator==(
		const Const_Ptr<
			signed char*, old_stack_begin, old_stack_end
		>&,
		const Const_Ptr<
			signed char*, old_stack_begin, old_stack_end
		>&
	);
#else
	template bool vm::operator==(
		const Const_Ptr<signed char*, old_stack_begin, old_ram_end>&,
		const Const_Ptr<signed char*, old_stack_begin, old_ram_end>&
	);
#endif

template bool vm::operator<(
	const Const_Ptr<
		const signed char*, old_code_begin, old_code_end
	>&,
	const Const_Ptr<const signed char*, old_code_begin, old_code_end>&
);

template bool vm::operator<(
	const Const_Ptr<
		signed char*, old_ram_begin, old_ram_end
	>&,
	const Const_Ptr<signed char*, old_ram_begin, old_ram_end>&
);

#if CONFIG_WITH_HEAP
	template bool vm::operator<(
		const Const_Ptr<
			signed char*, old_ram_begin, old_heap_end
		>&,
		const Const_Ptr<signed char*, old_ram_begin, old_heap_end>&
	);
#endif

#if CONFIG_WITH_CALL
	template bool vm::operator<(
		const Const_Ptr<
			signed char*, old_stack_begin, old_stack_end
		>&,
		const Const_Ptr<
			signed char*, old_stack_begin, old_stack_end
		>&
	);
#else
	template bool vm::operator<(
		const Const_Ptr<
			signed char*, old_stack_begin, old_ram_end
		>&,
		const Const_Ptr<
			signed char*, old_stack_begin, old_ram_end
		>&
	);
#endif

template class vm::Ptr<old_ram_begin, old_ram_end>;

#if CONFIG_WITH_HEAP
	template class vm::Casting_Ptr<old_ram_begin, old_heap_end>;
#endif

#if CONFIG_WITH_CALL
	template class vm::Casting_Ptr<
		old_stack_begin, old_stack_end
	>;
#else
	template class vm::Casting_Ptr<
		old_stack_begin, old_ram_end
	>;
#endif
