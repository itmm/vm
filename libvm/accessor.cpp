#include "accessor.h"

using namespace vm;

List Accessor::free_list;
List Accessor::alloc_list;

Value Accessor::pull() {
	auto value { get_value(Stack_Ptr { stack_begin }) };
	stack_begin += value_size(value); return value;
}

signed char Accessor::pull_ch() {
	auto value { pull() };
	auto ch = std::get_if<signed char>(&value);
	if (!ch) { err(Error::err_no_char); }
	return *ch;
}

int Accessor::pull_int() { return int_value(pull()); }


Heap_Ptr Accessor::pull_ptr() {
	auto value { pull() };
	auto ptr = std::get_if<Heap_Ptr>(&value);
	if (!ptr) { err(Error::err_no_pointer); }
	return *ptr;
}

void Accessor::push(Value value) {
	auto size { value_size(value) };
	if (heap_end + size > stack_begin) { err(Error::err_stack_overflow); }
	stack_begin -= size;
	auto ptr { Stack_Ptr { stack_begin } };
	set_value(ptr, value);
}

void Accessor::insert_into_free_list(Heap_Ptr block) {
	Heap_Ptr greater { };
	Heap_Ptr smaller { Accessor::free_list.begin };
	while (smaller && block < smaller) {
		greater = smaller;
		smaller = Accessor::get_ptr(smaller + node_next_offset);
	}
	int size { Accessor::get_int_value(block) };

	if (smaller) {
		int smaller_size { Accessor::get_int_value(smaller) };
		if (smaller + smaller_size == block) {
			Accessor::set_int(smaller, smaller_size + size);
			block = smaller; size += smaller_size;
			Accessor::free_list.remove(block);
		}
	}

	if (greater) {
		if (block + size == greater) {
			Accessor::set_int(block, size + Accessor::get_int_value(greater));
			auto old_greater { greater };
			greater = Accessor::get_ptr(greater + node_next_offset);
			Accessor::free_list.remove(old_greater);
		}
	}

	if (block.ptr_ + size == heap_end) {
		heap_end = block.ptr_;
	} else { Accessor::free_list.insert(block, greater); }
}

