#pragma once

#include <variant>

#include "ptr.h"

namespace vm {
	// TODO: add Stack Frame

	using Value = std::variant<signed char, int, Heap_Ptr>;

	int value_size(const Value& value);

	int int_value(const Value& value);

	signed char to_ch(int value, Err::Code overflow, Err::Code underflow);

}