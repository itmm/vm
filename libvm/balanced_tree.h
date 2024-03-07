#pragma once

#include "ordered_tree.h"

#if CONFIG_WITH_HEAP
	namespace vm {
		class Balanced_Tree: public Ordered_Tree {
			public:
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
				static Heap_Ptr move_red_left(Heap_Ptr node);
				static Heap_Ptr move_red_right(Heap_Ptr node);
				static void flip_colors(Heap_Ptr node);

			private:
				friend int testing::get_mark(const Heap_Ptr&);
				friend void testing::set_mark(Heap_Ptr, int);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
				static constexpr int black_mark { 1 };
				static constexpr int red_mark { -1 };
#pragma clang diagnostic pop

				static void set_size_with_mark(
					Heap_Ptr node, const Int& size, int mark
				);
				static int mark(const Heap_Ptr& node);
				static void set_mark(Heap_Ptr& node, int mark);
				static void toggle_mark(Heap_Ptr node);

				Heap_Ptr remove(const Heap_Ptr& node, Heap_Ptr parent);

				static Heap_Ptr remove_min(Heap_Ptr parent);

				static Heap_Ptr balance(Heap_Ptr parent);

				Heap_Ptr get_parent(const Heap_Ptr& a);
		};
	}
#endif