#include "gtest/gtest.h"

#include "heap.h"
#include "ordered_tree.h"
#include "vm-tests.h"
#include "balanced_tree.h"

using namespace vm;

#if CONFIG_WITH_HEAP
	TEST(tree_tests, empty) {
		signed char ram[0];
		const signed char code[] { };
		init(ram, ram + sizeof(ram), code, code + sizeof(code));
		Ordered_Tree tree;
		EXPECT_EQ(tree.smallest(), Heap_Ptr { });
		EXPECT_EQ(tree.greatest(), Heap_Ptr { });
	}

	namespace vm::testing {
		int get_mark(const Heap_Ptr& node) { return Tree::mark(node); }
		void set_mark(Heap_Ptr node, int mark) { Tree::set_mark(node, mark); }
	}

	TEST(tree_tests, switch_mark) {
		signed char ram[heap_overhead + 10];
		const signed char code[] { };
		init(ram, ram + sizeof(ram), code, code + sizeof(code));
		auto got { Heap::alloc_block(10, false) };
		auto raw { Heap::alloc_list.smallest() };
		EXPECT_EQ(raw + heap_overhead, got);
		EXPECT_EQ(Heap::alloc_list.greater(raw), Heap_Ptr { });
		EXPECT_EQ(Heap::alloc_list.smaller(raw), Heap_Ptr { });
		EXPECT_EQ(Tree::size(raw), Int { heap_overhead + 10 });
		EXPECT_EQ(vm::testing::get_mark(raw), 1);
		vm::testing::set_mark(raw, -1);
		EXPECT_EQ(vm::testing::get_mark(raw), -1);
		EXPECT_EQ(Tree::size(raw), Int { heap_overhead + 10 });
		Heap::free_block(got);
		EXPECT_EQ(Heap_Ptr::end, Ram_Ptr::begin);
	}

	TEST(tree_tests, keep_mark_on_size_change) {
		signed char ram[heap_overhead + 20];
		const signed char code[] { };
		init(ram, ram + sizeof(ram), code, code + sizeof(code));
		std::ignore = Heap::alloc_block(20, false);
		auto raw { Heap::alloc_list.smallest() };
		EXPECT_EQ(Tree::size(raw), Int { heap_overhead + 20 });
		vm::testing::set_mark(raw, -1);
		EXPECT_EQ(vm::testing::get_mark(raw), -1);
		EXPECT_EQ(Tree::size(raw), Int { heap_overhead + 20 });
		Tree::set_size(raw, Int { heap_overhead + 10 }, true);
		EXPECT_EQ(vm::testing::get_mark(raw), -1);
		EXPECT_EQ(Tree::size(raw), Int { heap_overhead + 10 });
	}
#endif