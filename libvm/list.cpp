#include "list.h"

#include "accessor.h"

using namespace vm;

#if CONFIG_WITH_HEAP
	void vm::List::insert(Heap_Ptr node) {
		Heap_Ptr smaller { end };
		Heap_Ptr greater;
		while (node < smaller) {
			greater = smaller;
			smaller = Acc::get_ptr(smaller + node_prev_offset);
		}

		Acc::set_ptr(node + node_next_offset, greater);
		Acc::set_ptr(node + node_prev_offset, smaller);
		if (greater) {
			Acc::set_ptr(greater + node_prev_offset, node);
		}
		else { end = node; }
		if (smaller) {
			Acc::set_ptr(smaller + node_next_offset, node);
		}
		else { begin = node; }
	}

	void List::remove(Heap_Ptr node) {
		auto next { Acc::get_ptr(node + node_next_offset) };
		auto prev { Acc::get_ptr(node + node_prev_offset) };
		if (next) { Acc::set_ptr(next + node_prev_offset, prev); }
		else { end = prev; }
		if (prev) { Acc::set_ptr(prev + node_next_offset, next); }
		else { begin = next; }
	}

	bool List::contains(Heap_Ptr node) const {
		auto current { begin };
		while (current && current < node) {
			current = Acc::get_ptr(current + node_next_offset);
		}
		return current == node;
	}
#endif