#include <cassert>

#include "accessor.h"
#include "balanced_tree.h"

using namespace vm;

#if CONFIG_WITH_HEAP
	namespace {
		constexpr int node_size_offset { 0 };
		constexpr int node_smaller_offset { Int::raw_size };
		constexpr int node_greater_offset { 2 * Int::raw_size };
	}

	void vm::Balanced_Tree::insert(Heap_Ptr node) {
		Ordered_Tree::insert(node); return;

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
			Acc::set_ptr(
				parent + node_smaller_offset,
				insert(node, Acc::get_ptr(parent + node_smaller_offset))
			);
		} else {
			Acc::set_ptr(
				parent + node_greater_offset,
				insert(node, Acc::get_ptr(parent + node_greater_offset))
			);
		}
		if (
			is_red(Acc::get_ptr(parent + node_greater_offset)) &&
			!is_red(Acc::get_ptr(parent + node_smaller_offset))
		) { parent = rotate_left(parent); }
		if (
			is_red(Acc::get_ptr(parent + node_smaller_offset)) &&
			is_red(Acc::get_ptr(Acc::get_ptr(parent + node_smaller_offset)) + node_smaller_offset)
		) { parent = rotate_right(parent); }
		if (
			is_red(Acc::get_ptr(parent + node_greater_offset)) &&
			is_red(Acc::get_ptr(parent + node_smaller_offset))
		) { flip_colors(parent); }
		return parent;
	}

	void Balanced_Tree::remove(Heap_Ptr node) {
		Ordered_Tree::remove(node); return;
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
		if (mark(node) == red_mark) { set_mark(node, black_mark); }
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
		assert(node);
		Heap_Ptr x { Acc::get_ptr(node + node_greater_offset) };
		assert(x);
		Acc::set_ptr(
			node + node_greater_offset, Acc::get_ptr(x + node_smaller_offset)
		);
		Acc::set_ptr(x + node_smaller_offset, node);
		set_mark(x, mark(node));
		set_mark(node, red_mark);
		return x;
	}

	Heap_Ptr Balanced_Tree::rotate_right(Heap_Ptr node) {
		assert(node);
		Heap_Ptr x { Acc::get_ptr(node + node_smaller_offset) };
		assert(x);
		Acc::set_ptr(node + node_smaller_offset, Acc::get_ptr(x + node_greater_offset));
		Acc::set_ptr(x + node_greater_offset, node);
		set_mark(x, mark(node));
		set_mark(node, red_mark);
		return x;
	}

	void Balanced_Tree::flip_colors(Heap_Ptr node) {
		assert(node);
		Heap_Ptr smaller { Acc::get_ptr(node + node_smaller_offset) };
		Heap_Ptr greater { Acc::get_ptr(node + node_greater_offset) };
		assert(smaller); assert(greater);
		set_mark(node, red_mark);
		set_mark(smaller, black_mark);
		set_mark(greater, black_mark);
	}
#endif