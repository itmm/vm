#pragma once

#include "ptr.h"
#include "vm.h"

#if CONFIG_WITH_HEAP
	namespace vm {

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

			private:
				Heap_Ptr root { };

				void insert_all(Heap_Ptr node);
		};

	}
#endif