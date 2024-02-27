#pragma once

#include "ptr.h"
#include "vm.h"

#if CONFIG_WITH_HEAP
	namespace vm {
		constexpr int node_next_offset { Int::raw_size };
		constexpr int node_prev_offset { 2 * Int::raw_size };
		constexpr int node_mark { 3 * Int::raw_size };

		struct List {
			// TODO: use red-black tree

			Heap_Ptr begin { };
			Heap_Ptr end { };

			void insert(Heap_Ptr node);
			void remove(Heap_Ptr node);
			[[nodiscard]] bool contains(Heap_Ptr node) const;
			[[nodiscard]] bool empty() const { return !begin; }
		};

	}
#endif