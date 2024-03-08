#pragma once

#include "balanced_tree.h"
#include "ptr.h"

#if CONFIG_WITH_HEAP
	namespace vm {
		class Heap {
			public:
				static Ordered_Tree free_list;
				static Ordered_Tree alloc_list;

				static void insert_into_free_list(Heap_Ptr block);
				static Heap_Ptr find_on_free_list(int size);
				[[nodiscard]] static Heap_Ptr alloc_block(
					int size, bool run_gc = true
				);

				static void free_block(Heap_Ptr block);

				static void dump_heap();

				static void collect_garbage();

			private:
				static Heap_Ptr find_on_free_list(int size, bool tight_fit);
		};
	}
#endif