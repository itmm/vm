#pragma once

#include "vm.h"

#define ID_(X) static_cast<signed char>(X)
#define PUSH_CH(X) vm::op_push_ch, ID_(X)
#define PUSH_SMALL_INT(X) PUSH_CH(X), vm::op_to_int

#define FOREACH_INT_(X, F) \
	F(((X) >> 24) & 0xff), F(((X) >> 16)& 0xff), \
	F(((X) >> 8) & 0xff), F((X) & 0xff)

#define RAW_CH(X) ch_type, ID_(X)
#define RAW_INT_(X) FOREACH_INT_(X, ID_)
#define RAW_INT(X) int_type, RAW_INT_(X)
#define RAW_PTR(X) ptr_type, RAW_INT_(X)
#define PUSH_INT(X) vm::op_push_int, RAW_INT(X)
