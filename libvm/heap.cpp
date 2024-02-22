#include <algorithm>
#include <cstring>
#include <iostream>
#include <set>

#include "accessor.h"
#include "heap.h"
#include "value.h"

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
			if (rest_size > heap_overhead) {
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void Heap::alloc_block(int orig_size, bool run_gc) {
	int size = std::max(orig_size + heap_overhead, node_size);
	auto found { find_on_free_list(size) };
	if (!found) {
		if (heap_end + size > stack_begin) {
			if (run_gc) {
				collect_garbage(); alloc_block(orig_size, false); return;
			}
			err(Err::heap_overflow);
		}
		found = Heap_Ptr { heap_end };
		heap_end += size;
		Acc::set_int(found, size);
	} else { size = Acc::get_int_value(found); }

	auto smaller = alloc_list.end;
	auto greater = Heap_Ptr { };
	while (found < smaller) {
		greater = smaller; smaller = Acc::get_ptr(smaller + node_prev_offset);
	}
	alloc_list.insert(found, greater);

	std::memset((found + heap_overhead).ptr_, 0, size - heap_overhead);
	Acc::push(found + heap_overhead);
}
#pragma clang diagnostic pop

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

template<typename P> void Heap::dump_block(P begin, P end, const char* indent) {
	P current { begin };
	while (current < end) {
		vm::Value value;
		try { value = Acc::get_value(current); }
		catch (const Err& err) {
			std::cout << indent << "! NO VALID VALUE AT " <<
				current.offset() << "\n";
			break;
		}
		if (auto ch { std::get_if<signed char>(&value) }) {
			std::cout << indent << current.offset() <<
				": char == " << static_cast<int>(*ch) << "\n";
			current = current + ch_size;
		} else if (auto val { std::get_if<int>(&value) }) {
			std::cout << indent << current.offset() <<
				": int == " << *val << "\n";
			current = current + int_size;
		} else if (auto ptr { std::get_if<Heap_Ptr>(&value) }) {
			std::cout << indent << current.offset() <<
				": ptr == " << ptr->offset() <<
				" (" << (*ptr ? (ptr->offset() - heap_overhead) : -1) << ")\n";
			current = current + ptr_size;
		} else {
			std::cout << indent << "! UNKNOWN TYPE AT " << current.offset() << "\n";
			break;
		}
	}
	if (!(current == end)) {
		std::cout << indent << "! INVALID END " << current.offset()<< " != " <<
			end.offset() << "\n";
	}
}

void Heap::dump_heap() {
	Heap_Ptr current { ram_begin };
	Heap_Ptr end { heap_end };
	Heap_Ptr next_allocated { alloc_list.begin };
	Heap_Ptr next_freed { free_list.begin };
	std::cout << "heap[" << heap_end - ram_begin << "] {\n";
	while (current < end) {
		auto size { Acc::get_int_value(current) };
		if (current == next_allocated) {
			std::cout << "  " << current.offset() << ": block[" << size << "] {\n";
			dump_block(current + heap_overhead, current + size, "    ");
			std::cout << "  }\n";
			next_allocated = Acc::get_ptr(next_allocated + node_next_offset);
		} else if (current == next_freed) {
			std::cout << "  " << current.offset() << ": free[" << size << "]\n";
			next_freed = Acc::get_ptr(next_freed + node_next_offset);
		} else {
			std::cout << "  ! INVALID BLOCK AT " << current.offset() << "\n";
		}
		current = current + size;
	}
	if (!(current == end)) {
		std::cout << "  ! INVALID HEAP END " <<
			current.offset() << " != " << end.offset() << "\n";
	}
	std::cout << "}\n";
}

void Heap::collect_garbage() {
	std::set<int> used;
	{
		Stack_Ptr current { stack_begin };
		Stack_Ptr end { ram_end };
		while (current < end) {
			auto value { Acc::get_value(current) };
			if (auto ptr = std::get_if<Heap_Ptr>(&value)) {
				used.insert(ptr->offset());
			}
			current = current + value_size(value);
		}
	}

	{
		Heap_Ptr current { alloc_list.begin };
		while (current) {
			auto next { Acc::get_ptr(current + node_next_offset) };
			if (used.find(current.offset() + heap_overhead) == used.end()) {
				free_block(current + heap_overhead);
			}
			current = next;
		}
	}

}

// instantiate templates

template void Heap::dump_block(
	Casting_Ptr<stack_begin, ram_end, Err::leave_stack_segment> begin,
	Casting_Ptr<stack_begin, ram_end, Err::leave_stack_segment> end,
	const char* indent
);
