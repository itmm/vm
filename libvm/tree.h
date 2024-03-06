#pragma once

#include "ptr.h"
#include "vm.h"

#if CONFIG_WITH_HEAP
	namespace vm {
		class Tree;

		namespace testing {
			int get_mark(const Heap_Ptr&);
			void set_mark(Heap_Ptr, int);
		}

		class Tree {
			public:
				// TODO: use red-black tree

				void insert(Heap_Ptr node);
				void remove(Heap_Ptr node);
				[[nodiscard]] bool contains(Heap_Ptr node) const;
				[[nodiscard]] bool empty() const { return !root; }
				[[nodiscard]] Heap_Ptr smaller(Heap_Ptr node) const;
				[[nodiscard]] Heap_Ptr greater(Heap_Ptr node) const;
				[[nodiscard]] Heap_Ptr smallest() const;
				[[nodiscard]] Heap_Ptr greatest() const;

				[[nodiscard]] static Int size(const Heap_Ptr& node);
				static void set_size(
					Heap_Ptr node, const Int& size, bool keep_mark = false
				);

				static Heap_Ptr rotate_left(Heap_Ptr node);
				static Heap_Ptr rotate_right(Heap_Ptr node);
				static void flip_colors(Heap_Ptr node);

			private:
				friend int testing::get_mark(const Heap_Ptr&);
				friend void testing::set_mark(Heap_Ptr, int);

				static constexpr int black_mark { 1 };
				static constexpr int red_mark { -1 };

				Heap_Ptr root { };

				void insert_all(Heap_Ptr node);
				static void set_size_with_mark(
					Heap_Ptr node, const Int& size, int mark
				);
				static int mark(const Heap_Ptr& node);
				static void set_mark(Heap_Ptr& node, int mark);
		};
	}

#endif