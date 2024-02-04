#pragma once

void expect_stack(
	const signed char* code_begin, int code_size,
	const signed char* expected_begin, int expected_size
);

#define EXPECT_STACK(CODE, EXPECTED) expect_stack(\
	CODE, sizeof(CODE), EXPECTED, sizeof(EXPECTED)\
)
