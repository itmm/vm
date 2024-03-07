#pragma once

#include "ordered_tree.h"

#if CONFIG_WITH_HEAP
	namespace vm {
		class Balanced_Tree;

		namespace testing {
			int get_mark(const Heap_Ptr&);
			void set_mark(Heap_Ptr, int);
		}

		class Balanced_Tree: public Ordered_Tree {
			public:
				// TODO: use red-black tree

				void insert(Heap_Ptr node) override;
				static Heap_Ptr insert(Heap_Ptr node, Heap_Ptr parent);
				void remove(Heap_Ptr node) override;

				[[nodiscard]] static Int size(const Heap_Ptr& node);
				static void set_size(
					Heap_Ptr node, const Int& size, bool keep_mark = false
				);

				static bool is_red(Heap_Ptr node);
				static Heap_Ptr rotate_left(Heap_Ptr node);
				static Heap_Ptr rotate_right(Heap_Ptr node);
				static void flip_colors(Heap_Ptr node);

			private:
				friend int testing::get_mark(const Heap_Ptr&);
				friend void testing::set_mark(Heap_Ptr, int);

				static constexpr int black_mark { 1 };
				static constexpr int red_mark { -1 };

				static void set_size_with_mark(
					Heap_Ptr node, const Int& size, int mark
				);
				static int mark(const Heap_Ptr& node);
				static void set_mark(Heap_Ptr& node, int mark);
		};
	}
#endif