#pragma once

#include "ptr.h"
#include "vm.h"

namespace vm {
	constexpr int node_next_offset { raw_int_size };
	constexpr int node_prev_offset { 2 * raw_int_size };

	struct List {
		Heap_Ptr begin { };
		Heap_Ptr end { };

		void insert(Heap_Ptr node);
		void remove(Heap_Ptr node);
		[[nodiscard]] bool contains(Heap_Ptr node) const;
		[[nodiscard]] bool empty() const { return !begin; }
	};

}
