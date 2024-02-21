#pragma once

#include "asm.h"
#include "err.h"

extern bool dump_heap;
extern bool dump_free;
extern bool dump_stack;

class Enable_Dump {
		bool old_heap, old_free, old_stack;

	public:
		Enable_Dump(bool heap, bool free, bool stack):
			old_heap { dump_heap }, old_free { dump_free },
			old_stack { dump_stack }
		{ dump_heap = heap; dump_free = free; dump_stack = stack; }

		~Enable_Dump() {
			dump_stack = old_stack; dump_free = old_free; dump_heap = old_heap;
		}
};

void expect_stack(
	const signed char* code_begin, int code_size,
	vm::Err::Code expected_error, int ram_size,
	const signed char* expected_begin, int expected_size
);

#define EXPECT_STACK(CODE, EXPECTED) expect_stack( \
	CODE, sizeof(CODE), vm::Err::leave_code_segment, \
	1024, EXPECTED, sizeof(EXPECTED) \
)

#define EXPECT_LIMITED_STACK(CODE, RAM, EXPECTED) expect_stack( \
	CODE, sizeof(CODE), vm::Err::leave_code_segment, \
	RAM, EXPECTED, sizeof(EXPECTED) \
)

#define EXPECT_LIMITED_STACK_ERROR(CODE, RAM, EXPECTED) expect_stack( \
	CODE, sizeof(CODE), EXPECTED, RAM, nullptr, 0 \
)

#define EXPECT_ERROR(CODE, EXPECTED) EXPECT_LIMITED_STACK_ERROR( \
	CODE, 1024, EXPECTED \
)

#define EXPECT_STACK_OVERFLOW(CODE, RAM) expect_stack( \
	CODE, sizeof(CODE), vm::Err::stack_overflow, RAM, nullptr, 0 \
)
