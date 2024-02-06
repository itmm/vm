#pragma once

#include "vm.h"

void expect_stack(
	const signed char* code_begin, int code_size,
	vm::Error::Code expected_error, int ram_size,
	const signed char* expected_begin, int expected_size
);

#define EXPECT_STACK(CODE, EXPECTED) expect_stack( \
	CODE, sizeof(CODE), vm::Error::err_leave_code_segment, \
	1024, EXPECTED, sizeof(EXPECTED) \
)

#define EXPECT_ERROR(CODE, EXPECTED) expect_stack( \
	CODE, sizeof(CODE), EXPECTED, 1024, nullptr, 0 \
)

#define ID_(X) static_cast<signed char>(X)
#define PUSH_CH(X) vm::op_push_ch, ID_(X)
#define PUSH_SMALL_INT(X) PUSH_CH(X), vm::op_ch_to_int

#define FOREACH_INT_(X, F) \
	F(((X) >> 24) & 0xff), F(((X) >> 16)& 0xff), \
	F(((X) >> 8) & 0xff), F((X) & 0xff)

#define RAW_INT(X) FOREACH_INT_(X, ID_)
#define PUSH_INT(X) vm::op_push_int, RAW_INT(X)
