#pragma once

#include "tree.h"
#include "ptr.h"

#if CONFIG_WITH_HEAP
	namespace vm {
		class Heap {
			public:
				static Tree free_list;
				static Tree alloc_list;

				static void insert_into_free_list(Heap_Ptr block);
				static Heap_Ptr find_on_free_list(int size);
				static void alloc_block(int size, bool run_gc = true);
				static void free_block(Heap_Ptr block);

				static void dump_heap();

				static void collect_garbage();

			private:
				static Heap_Ptr find_on_free_list(int size, bool tight_fit);
		};
	}
#endif