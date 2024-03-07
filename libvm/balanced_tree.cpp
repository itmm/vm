#include <cassert>

#include "accessor.h"
#include "balanced_tree.h"

using namespace vm;

#if CONFIG_WITH_HEAP
	namespace {
		constexpr int node_size_offset { 0 };

	}

	void Balanced_Tree::toggle_mark(Heap_Ptr node) {
		if (!node) { return; }
		set_mark(node, mark(node) == red_mark ? black_mark : red_mark);
	}

	void vm::Balanced_Tree::insert(Heap_Ptr node) {
		assert(node); if (!node) { return; }
		auto smaller { get_smaller(node) };
		auto greater { get_greater(node) };
		assert(!smaller && !greater);
		if (smaller || greater) { return; }
		root = insert(node, root);
		set_mark(root, black_mark);
	}

	bool vm::Balanced_Tree::is_red(Heap_Ptr node) {
		return node && mark(node) == red_mark;
	}

	Heap_Ptr vm::Balanced_Tree::insert(Heap_Ptr node, Heap_Ptr parent) {
		if (!node) { return parent; }
		if (!parent) { set_mark(node, red_mark); return node; }
		if (node < parent) {
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

	Heap_Ptr Balanced_Tree::move_red_left(Heap_Ptr node) {
		flip_colors(node);
		if (is_red(get_smaller(get_greater(node)))) {
			set_greater(node, rotate_right(get_greater(node)));
			node = rotate_left(node);
		}
		return node;
	}

	Heap_Ptr Balanced_Tree::move_red_right(Heap_Ptr node) {
		flip_colors(node);
		if (is_red(get_smaller(get_smaller(node)))) {
			node = rotate_right(node);
		}
		return node;
	}

	Heap_Ptr Balanced_Tree::balance(Heap_Ptr parent) {
		if (is_red(get_greater(parent))) { parent = rotate_left(parent); }
		auto smaller { get_smaller(parent) };
		if (is_red(smaller) && is_red(get_smaller(smaller))) { parent = rotate_right(parent); }
		if (is_red(get_smaller(parent)) && is_red(get_greater(parent))) { flip_colors(parent); }
		return parent;
	}

	Heap_Ptr Balanced_Tree::remove_min(Heap_Ptr parent) {
		auto smaller = get_smaller(parent);
		if (!smaller) { return Heap_Ptr { }; }
		if (!is_red(smaller) && !is_red(get_smaller(smaller))) {
			parent = move_red_left(parent);
		}
		set_smaller(parent, remove_min(get_smaller(parent)));
		return balance(parent);
	}

	Heap_Ptr Balanced_Tree::get_parent(const Heap_Ptr& node) {
		Heap_Ptr parent { };
		for (auto current { root }; !(current == node);) {
			parent = current;
			if (node < current) {
				current = get_smaller(current);
			} else {
				current = get_greater(current);
			}
			assert(current); if (!current) { return Heap_Ptr { }; }
		}
		return parent;
	}

	void Balanced_Tree::swap_nodes(const Heap_Ptr& a, const Heap_Ptr& b) {
		assert(a); assert(b);
		if (!a || !b) { return; }
		auto a_parent { get_parent(a) };
		auto b_parent { get_parent(b) };
		if (a_parent) {
			if (a < a_parent) {
				set_smaller(a_parent, b);
			} else {
				set_greater(a_parent, b);
			}
		} else if (root == a) { root = b; }

		if (b_parent) {
			if (b < b_parent) {
				set_smaller(b_parent, a);
			} else {
				set_greater(b_parent, b);
			}
		} else if (root == b) { root = a; }
	}

	Heap_Ptr Balanced_Tree::remove(const Heap_Ptr& node, Heap_Ptr parent) {
		assert(node); if (!node) { return Heap_Ptr { }; }
		if (node < parent) {
			auto smaller { get_smaller(parent) };
			if (!is_red(smaller) && !is_red(get_smaller(smaller))) {
				parent = move_red_left(parent);
			}
			set_smaller(parent, remove(node, get_smaller(parent)));
		} else {
			if (is_red(get_smaller(parent))) { parent = rotate_right(parent); }
			if (parent == node && !get_greater(parent)) {
				return Heap_Ptr { };
			}
			{
				auto greater { get_greater(parent) };
				if (!is_red(greater) && !is_red(get_smaller(greater))) {
					parent = move_red_right(parent);
				}
			}
			if (parent == node) {
				auto x { smallest(get_greater(parent)) };
				set_greater(parent, remove_min(get_greater(parent)));
				set_mark(x, mark(parent));
				auto p { get_parent(parent) };
				if (parent < p) {
					set_smaller(p, x);
				} else {
					set_greater(p, x);
				}
				set_smaller(x, get_smaller(parent));
				parent = x;
			} else {
				set_greater(parent, remove(node, get_greater(parent)));
			}

			return balance(parent);
		}

		auto smaller { get_smaller(node) };
		auto greater { get_greater(node) };
		if (smaller && greater) { return this->greater(greater); }
		if (!smaller && !greater) { return Heap_Ptr { }; }
		if (smaller) { return smaller; }
		return greater;
	}

	void Balanced_Tree::remove(const Heap_Ptr node) {
		assert(node); if (!node) { return; }
		if (!root) { return; }
		if (!is_red(get_smaller(root)) && !is_red(get_greater(root))) {
			set_mark(root, red_mark);
		}
		root = remove(node, root);
		if (root) { set_mark(root, black_mark); }
		set_smaller(node, Heap_Ptr { });
		set_greater(node, Heap_Ptr { });
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
		assert(smaller); assert(greater);
		if (! smaller || ! greater) { return; }
		toggle_mark(node);
		toggle_mark(smaller);
		toggle_mark(greater);
	}
#endif