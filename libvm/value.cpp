#include <limits>

#include <iostream>

#include "value.h"
#include "vm.h"

using namespace vm;

#if CONFIG_WITH_CALL
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
#endif

int vm::value_size(signed char type) {
	switch (type) {
		#if CONFIG_WITH_CHAR
			case Char::type_ch: return Char::typed_size;
		#endif
		#if CONFIG_WITH_INT
			case Int::type_ch: return Int::typed_size;
		#endif
		#if CONFIG_WITH_HEAP
			case ptr_type: return ptr_size;
		#endif
		#if CONFIG_WITH_CALL
			case stack_frame_type: return stack_frame_size;
		#endif
		default: err(Err::unknown_type);
	}
}

int vm::value_size(const Value& value) {
	#if CONFIG_WITH_CHAR
		if (std::get_if<Char>(&value)) {
			return Char::typed_size;
		}
	#endif
	#if CONFIG_WITH_INT
		if (std::get_if<Int>(&value)) {
			return Int::typed_size;
		}
	#endif
	#if CONFIG_WITH_HEAP
		if (std::get_if<Heap_Ptr>(&value)) {
			return ptr_size;
		}
	#endif
	#if CONFIG_WITH_CALL
		if (std::get_if<Stack_Frame>(&value)) {
			return stack_frame_size;
		}
	#endif
	err(Err::unknown_type);
}

#if CONFIG_WITH_INT
Int vm::int_value(const Value& value) {
	#if CONFIG_WITH_CHAR
		if (auto ch = std::get_if<Char>(&value)) {
			return Int { ch->value };
		}
	#endif
	if (auto val = std::get_if<Int>(&value)) {
		return *val;
	}
	err(Err::no_integer);
}
#endif

#if CONFIG_WITH_CHAR
	Char vm::to_ch(int value, Err::Code overflow, Err::Code underflow) {
		if (value > std::numeric_limits<signed char>::max()) { err(overflow); }
		if (value < std::numeric_limits<signed char>::min()) { err(underflow); }
		return Char { static_cast<signed char>(value) };
	}
#endif

