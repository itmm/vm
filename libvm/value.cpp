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
		#if CONFIG_WITH_BYTE
			case Byte::type_ch: return Byte::typed_size;
		#endif
		#if CONFIG_WITH_INT
			case Int::type_ch: return Int::typed_size;
		#endif
		#if CONFIG_WITH_HEAP
			case ptr_type: return ptr_size;
		#endif
		#if CONFIG_WITH_CALL
			case Stack_Frame::type_ch: return Stack_Frame::typed_size;
		#endif
		default: err(Err::unknown_type);
	}
}

int vm::value_size(const Value& value) {
	#if CONFIG_WITH_BYTE
		if (std::get_if<Byte>(&value)) {
			return Byte::typed_size;
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
			return Stack_Frame::typed_size;
		}
	#endif
	err(Err::unknown_type);
}

#if CONFIG_WITH_NUMERIC
	Int::internal_type vm::internal_int_value(const Value& value) {
		#if CONFIG_WITH_BYTE
			if (auto ch = std::get_if<Byte>(&value)) {
				return ch->value;
			}
		#endif
		#if CONFIG_WITH_INT
			if (auto val = std::get_if<Int>(&value)) {
				return val->value;
			}
		#endif
		err(Err::no_integer);
	}
#endif

#if CONFIG_WITH_INT
	#if !CONFIG_INTERNAL_INT_IS_INT
		Int vm::int_value(const Value& value) {
			#if CONFIG_WITH_BYTE
				if (auto ch = std::get_if<Byte>(&value)) {
					return Int { ch->value };
				}
			#endif
			if (auto val = std::get_if<Int>(&value)) {
				return *val;
			}
			err(Err::no_integer);
		}
	#endif
#endif

#if CONFIG_WITH_BYTE
	Byte vm::to_ch(int value, Err::Code overflow, Err::Code underflow) {
		if (value > std::numeric_limits<signed char>::max()) { err(overflow); }
		if (value < std::numeric_limits<signed char>::min()) { err(underflow); }
		return Byte { static_cast<signed char>(value) };
	}
#endif
