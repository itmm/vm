#pragma once

#include <variant>

#include "ptr.h"

namespace vm {
	using Value = std::variant<signed char, int, Heap_Ptr>;

	int value_size(const Value& value);

	int int_value(const Value& value);

	signed char to_ch(int value, Error::Code overflow, Error::Code underflow);

}