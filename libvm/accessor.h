#pragma once

#include "ptr.h"
#include "value.h"
#include "vm.h"

namespace vm {
	class Acc {
		public:
			template<typename T, T& B, T& E, Err::Code C>
			static int get_int_value(const Const_Ptr<T, B, E, C>& ptr);

			template<signed char*& B, signed char*& E, Err::Code C>
			static void set_int(Ptr<B, E, C> ptr, int value);

			template<typename T, T& B, T& E, Err::Code C>
			static signed char get_byte(const Const_Ptr<T, B, E, C>& ptr);

			template<typename T, T& B, T& E, Err::Code C>
			static Value get_value(const Const_Ptr<T, B, E, C>& ptr);

			template<signed char*& B, signed char*& E, Err::Code C>
			static void set_value(Ptr<B, E, C> ptr, const Value& value);

			template<typename P> static Heap_Ptr get_ptr(const P& ptr);

			template<typename P> static void set_ptr(
				P ptr, const Heap_Ptr& value
			);

			static Value pull();
			static signed char pull_ch();
			static int pull_int();
			static Heap_Ptr pull_ptr();

			static void push(Value value);
	};
}