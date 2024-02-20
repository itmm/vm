#include "list.h"

#include "accessor.h"

using namespace vm;

void vm::List::insert(Heap_Ptr node, Heap_Ptr next) {
	Heap_Ptr prev;
	if (next) { prev = Acc::get_ptr(next + node_prev_offset); }
	else { prev = end; }
	Acc::set_ptr(node + node_next_offset, next);
	Acc::set_ptr(node + node_prev_offset, prev);
	if (next) { Acc::set_ptr(next + node_prev_offset, node); }
	else { end = node; }
	if (prev) { Acc::set_ptr(prev + node_next_offset, node); }
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
