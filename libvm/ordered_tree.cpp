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
#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantConditionsOC"
			bool result {
				node && offset >= 0 && offset + Int::raw_size <= node_size
			};
#pragma clang diagnostic pop
			assert(result);
			return result;
		}

		Heap_Ptr get_offset(const Heap_Ptr& node, Int::internal_type offset) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantConditionsOC"
#pragma ide diagnostic ignored "UnreachableCode"
			if (!assert_node_offset(node, offset)) { return Heap_Ptr { }; }
#pragma clang diagnostic pop
			return Acc::get_ptr(node + offset);
		}

		void set_offset(
			Heap_Ptr node, Int::internal_type offset, const Heap_Ptr& child
		) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantConditionsOC"
#pragma ide diagnostic ignored "UnreachableCode"
			if (!assert_node_offset(node, offset)) { return; }
#pragma clang diagnostic pop
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

	void Ordered_Tree::remove(Heap_Ptr node) {
		assert(node); if (!node) { return; }
		Heap_Ptr upper; auto current = root;
		while (current && !(current == node)) {
			upper = current;
			current = Acc::get_ptr(current + ((node < current) ? node_smaller_offset : node_greater_offset));
		}
		if (!(current == node)) { return; }

		Heap_Ptr replacement;
		if (!get_smaller(node)) {
			replacement = get_greater(node); set_greater(node, Heap_Ptr { });
		}
		else if (!get_greater(node)) {
			replacement = get_smaller(node); set_smaller(node, Heap_Ptr { });
		} else {
			replacement = get_greater(node);
			auto x { smallest(replacement) };
			set_smaller(x, get_smaller(node));
			set_greater(node, Heap_Ptr { });
			set_smaller(node, Heap_Ptr { });
		}
		if (!upper) { root = replacement; }
		else if (node < upper) { set_smaller(upper, replacement); }
		else { set_greater(upper, replacement); }
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
				if (!candidate || candidate < current) { candidate = current; }
				current = get_greater(current);
			} else {
				current = get_smaller(current);
			}
		}
		return candidate;
	}

	Heap_Ptr Ordered_Tree::greater(Heap_Ptr node) const {
		Heap_Ptr candidate;
		auto current { root };
		while (current) {
			if (node < current) {
				if (!candidate || current < candidate) { candidate = current; }
				current = get_smaller(current);
			} else {
				current = get_greater(current);
			}
		}
		return candidate;
	}

	Heap_Ptr Ordered_Tree::smallest(Heap_Ptr current) {
		Heap_Ptr candidate { };
		while (current) {
			candidate = current;
			current = get_smaller(current);
		}
		return candidate;
	}

	Heap_Ptr Ordered_Tree::greatest() const {
		Heap_Ptr candidate;
		auto current { root };
		while (current) {
			candidate = current;
			current = get_greater(current);
		}
		return candidate;
	}
#endif