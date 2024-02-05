#pragma once

#include "vm.h"

void expect_stack(
	const signed char* code_begin, int code_size,
	const signed char* expected_begin, int expected_size
);

#define EXPECT_STACK(CODE, EXPECTED) expect_stack(\
	CODE, sizeof(CODE), EXPECTED, sizeof(EXPECTED)\
)

void expect_error(
	const signed char* code_begin, int code_size,
	vm::Error::Code expected_error
);

#define EXPECT_ERROR(CODE, EXPECTED) expect_error(\
	CODE, sizeof(CODE), EXPECTED\
)