#include <limits>

#include <iostream>

#include "value.h"
#include "vm.h"

using namespace vm;

bool vm::operator==(const Stack_Frame& a, const Stack_Frame& b) {
	return a.pc == b.pc && a.parent == b.parent && a.outer == b.outer;
}

bool vm::operator<(const Stack_Frame& a, const Stack_Frame& b) {
	return a.pc < b.pc || (
		a.pc == b.pc && (a.parent < b.parent || (
			a.parent == b.parent && a.outer < b.outer
		))
	);
}

int vm::value_size(signed char type) {
	switch (type) {
		case Char::type_ch: return Char::typed_size;
		case int_type: return int_size;
		case ptr_type: return ptr_size;
		case stack_frame_type: return stack_frame_size;
		default: err(Err::unknown_type);
	}
}

int vm::value_size(const Value& value) {
	if (std::get_if<signed char>(&value)) {
		return Char::typed_size;
	} else if (std::get_if<int>(&value)) {
		return int_size;
	} else if (std::get_if<Heap_Ptr>(&value)) {
		return ptr_size;
	} else if (std::get_if<Stack_Frame>(&value)) {
		return stack_frame_size;
	} else { err(Err::unknown_type); }
}

int vm::int_value(const Value& value) {
	if (auto ch = std::get_if<signed char>(&value)) {
		return *ch;
	} else if (auto val = std::get_if<int>(&value)) {
		return *val;
	} else { err(Err::no_integer); }
}

signed char vm::to_ch(int value, Err::Code overflow, Err::Code underflow) {
	if (value > std::numeric_limits<signed char>::max()) { err(overflow); }
	if (value < std::numeric_limits<signed char>::min()) { err(underflow); }
	return static_cast<signed char>(value);
}

