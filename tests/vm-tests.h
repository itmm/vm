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

#define ID_(X) static_cast<signed char>(X)
#define PUSH_CH(X) vm::op_push_ch, ID_(X)
#define PUSH_SMALL_INT(X) PUSH_CH(X), vm::op_ch_to_int

#define FOREACH_INT_(X, F) \
	F((X) & 0xff), F(((X) >> 8) & 0xff), \
	F(((X) >> 16) & 0xff), F(((X) >> 24)& 0xff)

#define FOREACH_INT_REV_(X, F) \
	F(((X) >> 24) & 0xff), F(((X) >> 16)& 0xff), \
	F(((X) >> 8) & 0xff), F((X) & 0xff)

#define PUSH_INT(X) FOREACH_INT_(X, PUSH_CH)

#define RAW_INT(X) FOREACH_INT_REV_(X, ID_)
