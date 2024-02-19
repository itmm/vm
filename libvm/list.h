#pragma once

#include "ptr.h"
#include "vm.h"

namespace vm {
	constexpr int node_next_offset { int_size };
	constexpr int node_prev_offset { 2 * int_size };

	struct List {
		Heap_Ptr begin { };
		Heap_Ptr end { };

		void insert(Heap_Ptr node, Heap_Ptr next);
		void remove(Heap_Ptr node);
	};

}