#pragma once

#include "vm.h"

#define ID_(X) static_cast<signed char>(X)

#if CONFIG_WITH_BYTE
	#define PUSH_BYTE(X) vm::op_push_ch, ID_(X)

	#define RAW_BYTE(X) vm::Byte::type_ch, ID_(X)
#endif

#define FOREACH_INT_(X, F) \
	F(((X) >> 24) & 0xff), F(((X) >> 16)& 0xff), \
	F(((X) >> 8) & 0xff), F((X) & 0xff)

#if CONFIG_WITH_INT
	#define RAW_INT_(X) FOREACH_INT_(X, ID_)
	#define RAW_INT(X) Int::type_ch, RAW_INT_(X)
	#define RAW_PTR(X) ptr_type, RAW_INT_(X)
	#define PUSH_INT(X) vm::op_push_int, RAW_INT_(X)
	#if CONFIG_WITH_BYTE
		#define PUSH_SMALL_INT(X) PUSH_BYTE(X), vm::op_to_int
	#else
		#define PUSH_SMALL_INT(X) PUSH_INT(X)
	#endif
#endif