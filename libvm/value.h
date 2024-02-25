#pragma once

#include <variant>

#include "ptr.h"

namespace vm {
	// TODO: add Stack Frame

	struct Stack_Frame {
		Code_Ptr pc;
		Ram_Ptr parent;
		Ram_Ptr outer;
	};

	bool operator==(const Stack_Frame& a, const Stack_Frame& b);
	bool operator<(const Stack_Frame& a, const Stack_Frame& b);

	using Value = std::variant<signed char, int, Heap_Ptr, Stack_Frame>;

	int value_size(signed char type);
	int value_size(const Value& value);

	int int_value(const Value& value);

	signed char to_ch(int value, Err::Code overflow, Err::Code underflow);

}