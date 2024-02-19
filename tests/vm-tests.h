#pragma once

#include "asm.h"
#include "err.h"

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
