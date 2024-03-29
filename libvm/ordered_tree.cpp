#include <cassert>

#include "accessor.h"
#include "ordered_tree.h"

using namespace vm;

#if CONFIG_WITH_HEAP
	namespace {
		constexpr Int::internal_type node_smaller_offset { Int::raw_size };
		constexpr Int::internal_type node_greater_offset { 2 * Int::raw_size };
		static_assert(node_size >= node_greater_offset + Int::raw_size);

		[[nodiscard]] bool assert_node_offset(
			const Heap_Ptr& node, Int::internal_type offset
		) {
			bool result {
				node && offset >= 0 && offset + Int::raw_size <= node_size
			};
			assert(result);
			return result;
		}

		Heap_Ptr get_offset(const Heap_Ptr& node, Int::internal_type offset) {
			if (!assert_node_offset(node, offset)) { return Heap_Ptr { }; }
			return Acc::get_ptr(node + offset);
		}

		void set_offset(
			Heap_Ptr node, Int::internal_type offset, const Heap_Ptr& child
		) {
			if (!assert_node_offset(node, offset)) { return; }
			Acc::set_ptr(node + offset, child);
		}

	}

	Heap_Ptr Ordered_Tree::get_smaller(const Heap_Ptr& node) {
		return get_offset(node, node_smaller_offset);
	}

	void Ordered_Tree::set_smaller(Heap_Ptr node, const Heap_Ptr& smaller) {
		set_offset(node, node_smaller_offset, smaller);
	}

	Heap_Ptr Ordered_Tree::get_greater(const Heap_Ptr& node) {
		return get_offset(node, node_greater_offset);
	}

	void Ordered_Tree::set_greater(Heap_Ptr node, const Heap_Ptr& greater) {
		set_offset(node, node_greater_offset, greater);
	}

	void vm::Ordered_Tree::init(Heap_Ptr node) {
		assert(node); if (!node) { return; }
		set_smaller(node, Heap_Ptr { });
		set_greater(node, Heap_Ptr { });
	}

	void vm::Ordered_Tree::insert(Heap_Ptr node) {
		assert(node); if (!node) { return; }
		auto smaller { get_smaller(node) };
		auto greater { get_greater(node) };
		assert(!smaller && !greater);
		if (smaller || greater) { return; }
		auto parent { root };
		if (!parent) { root = node; return; }
		for (;;) {
			assert(parent.offset() != node.offset());
			if (parent.offset() == node.offset()) { return; }
			if (node.offset() < parent.offset()) {
				auto next { get_smaller(parent) };
				if (next) { parent = next; } else {
					set_smaller(parent, node); return;
				}
			} else {
				auto next { get_greater(parent) };
				if (next) { parent = next; } else {
					set_greater(parent, node); return;
				}
			}
		}
	}

	void Ordered_Tree::insert_all(vm::Heap_Ptr node) {
		if (! node) { return; }
		insert_all(get_smaller(node));
		insert_all(get_greater(node));
		set_smaller(node, Heap_Ptr { }); set_greater(node, Heap_Ptr { });
		insert(node);
	}

	void Ordered_Tree::remove(Heap_Ptr node) {
		assert(node); if (!node) { return; }
		Heap_Ptr upper; auto current = root;
		while (current && !(current == node)) {
			upper = current;
			current = Acc::get_ptr(current + ((node < current) ? node_smaller_offset : node_greater_offset));
		}
		if (!(current == node)) { return; }
		if (upper) {
			Acc::set_ptr(upper + ((node < upper) ? node_smaller_offset : node_greater_offset), Heap_Ptr { });
		} else { root = Heap_Ptr(); }
		insert_all(get_smaller(node));
		insert_all(get_greater(node));
		set_smaller(node, Heap_Ptr { });
		set_greater(node, Heap_Ptr { });
	}

	bool Ordered_Tree::contains(Heap_Ptr node) const {
		auto current { root };
		while (current) {
			if (current == node) { return true; }
			current = Acc::get_ptr(current + ((node < current) ? node_smaller_offset : node_greater_offset));
		}
		return false;
	}

	Heap_Ptr Ordered_Tree::smaller(Heap_Ptr node) const {
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

	Heap_Ptr Ordered_Tree::greater(Heap_Ptr node) const {
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

	Heap_Ptr Ordered_Tree::smallest() const {
		Heap_Ptr candidate { };
		auto current { root };
		while (current) {
			candidate = current;
			current = Acc::get_ptr(current + node_smaller_offset);
		}
		return candidate;
	}

	Heap_Ptr Ordered_Tree::greatest() const {
		Heap_Ptr candidate;
		auto current { root };
		while (current) {
			candidate = current;
			current = Acc::get_ptr(current + node_greater_offset);
		}
		return candidate;
	}
#endif