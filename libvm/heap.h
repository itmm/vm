#pragma once

#include "list.h"
#include "ptr.h"

#if CONFIG_WITH_HEAP
	namespace vm {
		class Heap {
			public:
				static List free_list;
				static List alloc_list;

				static void insert_into_free_list(Heap_Ptr block);
				static Heap_Ptr find_on_free_list(int size, bool tight_fit);
				static Heap_Ptr find_on_free_list(int size);
				static void alloc_block(int size, bool run_gc = true);
				static void free_block(Heap_Ptr block);

				static void dump_heap();

				static void collect_garbage();
		};
	}
#endif