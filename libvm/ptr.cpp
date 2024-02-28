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

template<typename T>
void Const_Ptr<T>::internal_check(int size, const signed char* begin, const signed char* end, Err::Code code) const {
	if (ptr_ < begin || ptr_ + size > end) { err(code); }
}

template<typename T>
int Const_Ptr<T>::internal_offset(const signed char* begin) const {
	return ptr_ ? static_cast<int>(ptr_ - begin) : -1;
}

template<typename P> P vm::operator+(const P& ptr, int offset) {
	return P { ptr.ptr_ + offset };
}

template<typename P> P vm::operator-(const P& ptr, int offset) {
	return P { ptr.ptr_ - offset };
}

template<typename T>
bool vm::operator==(const Const_Ptr<T>& a, const Const_Ptr<T>& b) {
	return a.ptr_ == b.ptr_;
}

template<typename T>
bool vm::operator<(const Const_Ptr<T>& a, const Const_Ptr<T>& b) {
	return a.ptr_ < b.ptr_;
}

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

template class vm::Const_Ptr<const signed char*>;

template void vm::Const_Ptr<signed char*>::internal_check(int, const signed char*, const signed char*, Err::Code) const;
template int vm::Const_Ptr<signed char*>::internal_offset(const signed char*) const;

template Code_Ptr vm::operator+(const Code_Ptr&, int);
template Const_Ptr<const signed char*> vm::operator+(const Const_Ptr<const signed char*>&, int);
template Ram_Ptr vm::operator+(const Ram_Ptr&, int);
template Const_Ptr<signed char*> vm::operator+(const Const_Ptr<signed char*>&, int);
template Const_Ptr<signed char*> vm::operator-(const Const_Ptr<signed char*>&, int);

#if CONFIG_WITH_HEAP
	template Heap_Ptr vm::operator+(const Heap_Ptr&, int);
	template Heap_Ptr vm::operator-(const Heap_Ptr&, int);
#endif

template Stack_Ptr vm::operator+(const Stack_Ptr&, int);
template Stack_Ptr vm::operator-(const Stack_Ptr&, int);

template bool vm::operator==(
	const Const_Ptr<const signed char*>&, const Const_Ptr<const signed char*>&
);

template bool vm::operator==(
	const Const_Ptr<signed char*>&, const Const_Ptr<signed char*>&
);

template bool vm::operator<(
	const Const_Ptr<const signed char*>&, const Const_Ptr<const signed char*>&
);

template bool vm::operator<(
	const Const_Ptr<signed char*>&, const Const_Ptr<signed char*>&
);
