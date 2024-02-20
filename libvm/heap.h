#pragma once

#include "ptr.h"

namespace vm {
	class Heap {
		public:
			static void insert_into_free_list(Heap_Ptr block);
	};
}
