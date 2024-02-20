#include "accessor.h"

using namespace vm;

List Acc::free_list;
List Acc::alloc_list;

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
	auto ptr { Stack_Ptr { stack_begin } };
	set_value(ptr, value);
}

void Acc::insert_into_free_list(Heap_Ptr block) {
	Heap_Ptr greater { };
	Heap_Ptr smaller { Acc::free_list.begin };
	while (smaller && block < smaller) {
		greater = smaller;
		smaller = Acc::get_ptr(smaller + node_next_offset);
	}
	int size { Acc::get_int_value(block) };

	if (smaller) {
		int smaller_size { Acc::get_int_value(smaller) };
		if (smaller + smaller_size == block) {
			Acc::set_int(smaller, smaller_size + size);
			block = smaller; size += smaller_size;
			Acc::free_list.remove(block);
		}
	}

	if (greater) {
		if (block + size == greater) {
			Acc::set_int(block, size + Acc::get_int_value(greater));
			auto old_greater { greater };
			greater = Acc::get_ptr(greater + node_next_offset);
			Acc::free_list.remove(old_greater);
		}
	}

	if (block.ptr_ + size == heap_end) {
		heap_end = block.ptr_;
	} else { Acc::free_list.insert(block, greater); }
}

