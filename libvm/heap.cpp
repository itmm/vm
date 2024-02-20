#include "accessor.h"
#include "heap.h"

using namespace vm;

void Heap::insert_into_free_list(Heap_Ptr block) {
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

