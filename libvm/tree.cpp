#include "tree.h"

#include "accessor.h"

using namespace vm;

#if CONFIG_WITH_HEAP
	namespace {
		constexpr int node_smaller_offset { Int::raw_size };
		constexpr int node_greater_offset { 2 * Int::raw_size };
	}

	void vm::Tree::insert(Heap_Ptr node) {
		if (!node) { return; }
		Acc::set_ptr(node + node_smaller_offset, Heap_Ptr { });
		Acc::set_ptr(node + node_greater_offset, Heap_Ptr { });
		auto current = root;
		if (!current) { root = node; return; }
		do {
			if (node < current) {
				auto smaller { Acc::get_ptr(current + node_smaller_offset) };
				if (smaller) { current = smaller; continue; }
				Acc::set_ptr(current + node_smaller_offset, node);
			} else {
				auto greater { Acc::get_ptr(current + node_greater_offset) };
				if (greater) { current = greater; continue; }
				Acc::set_ptr(current + node_greater_offset, node);

			}
		} while (false);
	}

	void Tree::insert_all(vm::Heap_Ptr node) {
		if (! node) { return; }
		insert_all(Acc::get_ptr(node + node_smaller_offset));
		insert_all(Acc::get_ptr(node + node_greater_offset));
		insert(node);
	}

	void Tree::remove(Heap_Ptr node) {
		if (!node) { return; }
		Heap_Ptr upper; auto current = root;
		while (current && !(current == node)) {
			upper = current;
			current = Acc::get_ptr(current + ((node < current) ? node_smaller_offset : node_greater_offset));
		}
		if (!(current == node)) { return; }
		if (upper) {
			Acc::set_ptr(upper + ((node < upper) ? node_smaller_offset : node_greater_offset), Heap_Ptr { });
		} else { root = Heap_Ptr(); }
		insert_all(Acc::get_ptr(node + node_smaller_offset));
		insert_all(Acc::get_ptr(node + node_greater_offset));
	}

	bool Tree::contains(Heap_Ptr node) const {
		auto current { root };
		while (current) {
			if (current == node) { return true; }
			current = Acc::get_ptr(current + ((node < current) ? node_smaller_offset : node_greater_offset));
		}
		return false;
	}

	Heap_Ptr Tree::smaller(Heap_Ptr node) const {
		Heap_Ptr candidate;
		auto current { root };
		while (current) {
			if (current < node) {
				if (candidate < current) { candidate = current; }
				current = Acc::get_ptr(current + node_greater_offset);
			} else {
				current = Acc::get_ptr(current + node_smaller_offset);
			}
		}
		return candidate;
	}

	Heap_Ptr Tree::greater(Heap_Ptr node) const {
		Heap_Ptr candidate;
		auto current { root };
		while (current) {
			if (current < node) {
				current = Acc::get_ptr(current + node_greater_offset);
			} else {
				if (current < candidate) { candidate = current; }
				current = Acc::get_ptr(current + node_smaller_offset);
			}
		}
		return candidate;
	}

	Heap_Ptr Tree::smallest() const {
		Heap_Ptr candidate { };
		auto current { root };
		while (current) {
			candidate = current;
			current = Acc::get_ptr(current + node_smaller_offset);
		}
		return candidate;
	}

	Heap_Ptr Tree::greatest() const {
		Heap_Ptr candidate;
		auto current { root };
		while (current) {
			candidate = current;
			current = Acc::get_ptr(current + node_greater_offset);
		}
		return candidate;
	}

#endif