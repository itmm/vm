#pragma once

#include "ptr.h"
#include "vm.h"

#if CONFIG_WITH_HEAP
	namespace vm {
		class Ordered_Tree;

		namespace testing {
			int get_mark(const Heap_Ptr&);
			void set_mark(Heap_Ptr, int);
		}

		class Ordered_Tree {
			public:
				virtual ~Ordered_Tree() = default;
				static void init(Heap_Ptr node);
				virtual void insert(Heap_Ptr node);
				virtual void remove(Heap_Ptr node);
				[[nodiscard]] bool contains(Heap_Ptr node) const;
				[[nodiscard]] explicit operator bool() const { return static_cast<bool>(root); }
				[[nodiscard]] Heap_Ptr smaller(Heap_Ptr node) const;
				[[nodiscard]] Heap_Ptr greater(Heap_Ptr node) const;
				[[nodiscard]] Heap_Ptr smallest() const;
				[[nodiscard]] Heap_Ptr greatest() const;

			protected:
				Heap_Ptr root { };

				void insert_all(Heap_Ptr node);
		};
	}
#endif