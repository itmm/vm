#include <limits>

#include "value.h"
#include "vm.h"

using namespace vm;

int vm::value_size(const Value& value) {
	if (std::get_if<signed char>(&value)) {
		return ch_size;
	} else if (std::get_if<int>(&value)) {
		return int_size;
	} else if (std::get_if<signed char*>(&value)) {
		return ptr_size;
	} else { err(Error::err_unknown_type); }
}

int vm::int_value(const Value& value) {
	if (auto ch = std::get_if<signed char>(&value)) {
		return *ch;
	} else if (auto val = std::get_if<int>(&value)) {
		return *val;
	} else { err(Error::err_no_integer); }
}

signed char vm::to_ch(int value, Error::Code overflow, Error::Code underflow) {
	if (value > std::numeric_limits<signed char>::max()) { err(overflow); }
	if (value < std::numeric_limits<signed char>::min()) { err(underflow); }
	return static_cast<signed char>(value);
}

