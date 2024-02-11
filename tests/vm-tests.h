#pragma once

#include "asm.h"

void expect_stack(
	const signed char* code_begin, int code_size,
	vm::Error::Code expected_error, int ram_size,
	const signed char* expected_begin, int expected_size
);

#define EXPECT_STACK(CODE, EXPECTED) expect_stack( \
	CODE, sizeof(CODE), vm::Error::err_leave_code_segment, \
	1024, EXPECTED, sizeof(EXPECTED) \
)

#define EXPECT_LIMITED_STACK(CODE, RAM, EXPECTED) expect_stack( \
	CODE, sizeof(CODE), vm::Error::err_leave_code_segment, \
	RAM, EXPECTED, sizeof(EXPECTED) \
)

#define EXPECT_ERROR(CODE, EXPECTED) expect_stack( \
	CODE, sizeof(CODE), EXPECTED, 1024, nullptr, 0 \
)

#define EXPECT_STACK_OVERFLOW(CODE, RAM) expect_stack( \
	CODE, sizeof(CODE), vm::Error::err_leave_stack_segment, RAM, nullptr, 0 \
)