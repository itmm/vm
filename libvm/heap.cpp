#include <algorithm>
#include <cstring>

#include "accessor.h"
#include "heap.h"

using namespace vm;

List Heap::free_list;
List Heap::alloc_list;

void Heap::insert_into_free_list(Heap_Ptr block) {
	Heap_Ptr greater { };
	Heap_Ptr smaller { free_list.begin };
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
			free_list.remove(block);
		}
	}

	if (greater) {
		if (block + size == greater) {
			Acc::set_int(block, size + Acc::get_int_value(greater));
			auto old_greater { greater };
			greater = Acc::get_ptr(greater + node_next_offset);
			free_list.remove(old_greater);
		}
	}

	if (block.ptr_ + size == heap_end) {
		heap_end = block.ptr_;
	} else { free_list.insert(block, greater); }
}

Heap_Ptr Heap::find_on_free_list(int size, bool tight_fit) {
	auto current { free_list.end };
	while (current) {
		int cur_size { Acc::get_int_value(current) };
		bool found {
			tight_fit ?
			cur_size == size || cur_size > 3 * size :
			cur_size >= size
		};
		if (found) {
			int rest_size { cur_size - size };
			if (rest_size >= heap_overhead) {
				Heap_Ptr rest_block { current + size };
				Acc::set_int(rest_block, rest_size);
				Acc::set_int(current, size);
				free_list.insert(rest_block, current);
			}
			free_list.remove(current);
			return current;
		}
		current = Acc::get_ptr(current + node_prev_offset);
	}
	return Heap_Ptr { };
}

Heap_Ptr Heap::find_on_free_list(int size) {
	auto got { find_on_free_list(size, true) };
	if (! got) { got = find_on_free_list(size, false); }
	return got;
}

void Heap::alloc_block(int size) {
	size = std::max(size + heap_overhead, node_size);
	auto found { find_on_free_list(size) };
	if (!found) {
		if (heap_end + size > stack_begin) { err(Err::heap_overflow); }
		found = Heap_Ptr { heap_end };
		heap_end += size;
		Acc::set_int(found, size);
	}
	alloc_list.insert(found, alloc_list.begin);
	std::memset((found + heap_overhead).ptr_, 0, size - heap_overhead);
	Acc::push(found + heap_overhead);
}

void Heap::free_block(Heap_Ptr block) {
	block = block - heap_overhead;
	alloc_list.remove(block);
	int size { Acc::get_int_value(block) };
	if (size < std::max(node_size, heap_overhead)) {
		err(Err::free_invalid_block);
	}
	if (Heap_Ptr { heap_end } < block + size) {
		err(Err::free_invalid_block);
	}
	Heap::insert_into_free_list(block);
}
