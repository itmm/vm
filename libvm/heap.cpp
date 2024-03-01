#include <algorithm>
#include <cstring>
#include <iostream>

#include "accessor.h"
#include "heap.h"
#include "value.h"

using namespace vm;

#if CONFIG_WITH_HEAP
	Tree Heap::free_list;
	Tree Heap::alloc_list;

	void Heap::insert_into_free_list(Heap_Ptr block) {
		free_list.insert(block);
		auto size { Tree::size(block).value };

		if (auto smaller = free_list.smaller(block)) {
			auto smaller_size { Tree::size(smaller).value };
			if (smaller + smaller_size == block) {
				Tree::set_size(smaller, Int { smaller_size + size });
				free_list.remove(block);
				block = smaller; size += smaller_size;
			}
		}

		if (auto greater = free_list.greater(block)) {
			if (block + size == greater) {
				Tree::set_size(block, Int { size + Tree::size(greater).value });
				free_list.remove(greater);
			}
		}

		if (block.ptr_ + size == Heap_Ptr::end) {
			free_list.remove(block);
			Heap_Ptr::end = block.ptr_;
		}
	}

	Heap_Ptr Heap::find_on_free_list(int size, bool tight_fit) {
		for (
			auto current = free_list.greatest();
			current; current = free_list.smaller(current)
		) {
			int cur_size { Tree::size(current).value };
			bool found {
				tight_fit ?
					cur_size == size || cur_size > 3 * size :
					cur_size >= size
			};
			if (found) {
				int rest_size { cur_size - size };
				if (rest_size > heap_overhead) {
					Heap_Ptr rest_block { current + size };
					Tree::set_size(rest_block, Int { rest_size });
					Tree::set_size(current, Int { size });
					free_list.insert(rest_block);
				}
				free_list.remove(current);
				return current;
			}
		}
		return Heap_Ptr { };
	}

	Heap_Ptr Heap::find_on_free_list(int size) {
		auto got { find_on_free_list(size, true) };
		if (! got) { got = find_on_free_list(size, false); }
		return got;
	}

	#pragma clang diagnostic push
	#pragma ide diagnostic ignored "misc-no-recursion"
	Heap_Ptr Heap::alloc_block(int orig_size, bool run_gc) {
		int size = std::max(orig_size + heap_overhead, node_size);
		auto found { find_on_free_list(size) };
		if (!found) {
			if (Heap_Ptr::end + size > Stack_Ptr::begin) {
				if (run_gc) {
					collect_garbage();
					return alloc_block(orig_size, false);
				}
				err(Err::heap_overflow);
			}
			found = Heap_Ptr { Heap_Ptr::end };
			Heap_Ptr::end += size;
			Tree::set_size(found, Int { size });
		} else { size = Tree::size(found).value; }

		alloc_list.insert(found);

		std::memset((found + heap_overhead).ptr_, 0, size - heap_overhead);
		return found + heap_overhead;
	}
	#pragma clang diagnostic pop

	void Heap::free_block(Heap_Ptr block) {
		block = block - heap_overhead;
		alloc_list.remove(block);
		auto size { Tree::size(block).value };
		if (size < std::max(node_size, heap_overhead)) {
			err(Err::free_invalid_block);
		}
		if (Heap_Ptr { Heap_Ptr::end } < block + size) {
			err(Err::free_invalid_block);
		}
		Heap::insert_into_free_list(block);
	}

	void Heap::dump_heap() {
		Heap_Ptr current { Ram_Ptr::begin };
		Heap_Ptr end { Heap_Ptr::end };
		Heap_Ptr next_allocated { alloc_list.smallest() };
		Heap_Ptr next_freed { free_list.smallest() };
		std::cout << "heap[" << Heap_Ptr::end - Ram_Ptr::begin << "] {";
		if (Heap_Ptr::end - Ram_Ptr::begin) {
			std::cout << "\n";
			while (current < end) {
				auto size { Tree::size(current).value };
				if (current == next_allocated) {
					std::cout << "  " << current.offset() <<
						": block[" << size << "] {";
					if (size) {
						std::cout << "\n";
						dump_block(
							current + heap_overhead, current + size, "    "
						);
						std::cout << "  }\n";
					} else { std::cout << " }\n"; }
					next_allocated = alloc_list.greater(next_allocated);
				} else if (current == next_freed) {
					std::cout << "  " << current.offset() <<
							  ": free[" << size << "] { }\n";
					next_freed = free_list.greater(next_freed);
				} else {
					std::cout << "  ! INVALID BLOCK AT " <<
						current.offset() << "\n";
				}
				current = current + size;
			}
			if (!(current == end)) {
				std::cout << "  ! INVALID HEAP END " <<
						  current.offset() << " != " << end.offset() << "\n";
			}
			std::cout << "}\n";
		} else { std::cout << " }\n"; }
	}

	template<typename P>
	static void add_pointers(P begin, P end, Tree& used_blocks) {
		P current { begin };
		while (current < end) {
			auto value { Acc::get_value(current) };
			if (auto ptr = std::get_if<Heap_Ptr>(&value)) {
				auto raw_ptr = *ptr - heap_overhead;
				if (Heap::alloc_list.contains(raw_ptr)) {
					Heap::alloc_list.remove(raw_ptr);
					used_blocks.insert(raw_ptr);
				}
			}
			current = current + value_size(value);
		}
	}

	class Full_Stack {
		public:
			Full_Stack():
				prev_stack_end {
					static_cast<int>(Stack_Ptr::end - Stack_Ptr::begin)
				}
			{ Stack_Ptr::end = Ram_Ptr::end; }

			~Full_Stack() {
				Stack_Ptr::end = Stack_Ptr::begin + prev_stack_end;
			}
		private:
			int prev_stack_end;
	};

	void Heap::collect_garbage() {
		Tree used_blocks;
		Tree processed_blocks;

		{
			Full_Stack full_stack;
			add_pointers(
				Stack_Ptr { Stack_Ptr::begin }, Stack_Ptr { Stack_Ptr::end },
				used_blocks
			);
		}

		while (!used_blocks.empty()) {
			Heap_Ptr current { used_blocks.smallest() };
			used_blocks.remove(current);
			processed_blocks.insert(current);
			Heap_Ptr end { current + Tree::size(current) };
			add_pointers(current + heap_overhead, end, used_blocks);
		}

		while (!alloc_list.empty()) {
			free_block(alloc_list.smallest() + heap_overhead);
		}

		alloc_list =  processed_blocks;
	}
#endif