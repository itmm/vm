#include <cassert>

#include "accessor.h"
#include "balanced_tree.h"

using namespace vm;

#if CONFIG_WITH_HEAP
	namespace {
		constexpr int node_size_offset { 0 };
	}

	void vm::Balanced_Tree::insert(Heap_Ptr node) {
		root = insert(node, root);
		set_mark(root, black_mark);
	}

	bool vm::Balanced_Tree::is_red(Heap_Ptr node) {
		return node && mark(node) == red_mark;
	}

	Heap_Ptr vm::Balanced_Tree::insert(Heap_Ptr node, Heap_Ptr parent) {
		if (!node) { return parent; }
		if (!parent) { set_mark(node, red_mark); return node; }
		if (node.offset() < parent.offset()) {
			set_smaller(parent, insert(node, get_smaller(parent)));
		} else {
			set_greater(parent, insert(node, get_greater(parent)));
		}
		if (is_red(get_greater(parent)) && !is_red(get_smaller(parent))) {
			parent = rotate_left(parent);
		}
		auto smaller { get_smaller(parent) };
		if (is_red(smaller) && is_red(get_smaller(smaller))) {
			parent = rotate_right(parent);
		}
		if (is_red(get_smaller(parent)) && is_red(get_greater(parent))) {
			flip_colors(parent);
		}
		return parent;
	}

	void Balanced_Tree::remove(Heap_Ptr node) {
		Ordered_Tree::remove(node); return;
	}

	Int Balanced_Tree::size(const Heap_Ptr& node) {
		assert(node); if (!node) { return Int { -1 }; }
		auto res { Acc::get_int(node + node_size_offset) };
		// TODO: check for max min
		if (res.value < 0) { res.value *= -1; }
		return res;
	}

	void Balanced_Tree::set_size_with_mark(
		vm::Heap_Ptr node, const vm::Int& size, int mark
	) {
		assert(node && size.value >= node_size && (mark == red_mark || mark == black_mark));
		if (!node || size.value < node_size || (mark != 1 && mark != -1)) {
			return;
		}

		Acc::set_int(node + node_size_offset, Int { size.value * mark });
	}

	int Balanced_Tree::mark(const Heap_Ptr& node) {
		assert(node); if (!node) { return black_mark; }
		auto res { Acc::get_int(node + node_size_offset) };
		return res.value >= 0 ? black_mark : red_mark;
	}

	void Balanced_Tree::set_size(Heap_Ptr node, const Int& size, bool keep_mark) {
		int m = black_mark;
		if (keep_mark) { m = mark(node); }
		set_size_with_mark(node, size, m);
	}

	void Balanced_Tree::set_mark(Heap_Ptr& node, int mark) {
		set_size_with_mark(node, size(node), mark);
	}

	Heap_Ptr Balanced_Tree::rotate_left(Heap_Ptr node) {
		assert(node); if (!node) { return Heap_Ptr { }; }
		Heap_Ptr x { get_greater(node) };
		assert(x); if (!x) { return Heap_Ptr { }; }
		set_greater(node, get_smaller(x));
		set_smaller(x, node);
		set_mark(x, mark(node));
		set_mark(node, red_mark);
		return x;
	}

	Heap_Ptr Balanced_Tree::rotate_right(Heap_Ptr node) {
		assert(node); if (!node) { return Heap_Ptr { }; }
		Heap_Ptr x { get_smaller(node) };
		assert(x); if (!x) { return Heap_Ptr { }; }
		set_smaller(node, get_greater(x));
		set_greater(x, node);
		set_mark(x, mark(node));
		set_mark(node, red_mark);
		return x;
	}

	void Balanced_Tree::flip_colors(Heap_Ptr node) {
		assert(node); if (! node) { return; }
		Heap_Ptr smaller { get_smaller(node) };
		Heap_Ptr greater { get_greater(node) };
		assert(smaller); assert(greater); if (! smaller || ! greater) { return; }
		set_mark(node, red_mark);
		set_mark(smaller, black_mark);
		set_mark(greater, black_mark);
	}
#endif