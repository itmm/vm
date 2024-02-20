#pragma once

#include "list.h"
#include "ptr.h"
#include "value.h"
#include "vm.h"

namespace vm {
	class Acc {
		public:
			template<typename T, T& B, T& E, Err::Code C>
			static int get_int_value(const Const_Ptr<T, B, E, C>& ptr) {
				int value { 0 };
				for (
					auto i { ptr.ptr_ }, e { ptr.ptr_ + raw_int_size };
					i < e; ++i
				) { value = (value << bits_per_byte) + (*i & byte_mask); }
				return value;
			}

			template<signed char*& B, signed char*& E, Err::Code C>
			static void set_int(Ptr<B, E, C> ptr, int value) {
				for (
					auto i { ptr.ptr_ + raw_int_size - 1 }; i >= ptr.ptr_; --i
				) {
					*i = static_cast<signed char>(value);
					value >>= bits_per_byte;
				}
				if (value != 0 && value != -1) { err(Err::int_overflow); }
			}

			template<typename T, T& B, T& E, Err::Code C>
			static signed char get_byte(const Const_Ptr<T, B, E, C>& ptr) {
				ptr.check(1); return *ptr.ptr_;
			}

			template<typename T, T& B, T& E, Err::Code C>
			static Value get_value(const Const_Ptr<T, B, E, C>& ptr) {
				ptr.check(1); switch (*ptr.ptr_) {
					case ch_type:
						ptr.check(ch_size); return Value { ptr.ptr_[1] };

					case int_type:
						ptr.check(int_size);
						return Value { get_int_value(ptr + 1) };

					case ptr_type: {
						ptr.check(ptr_size);
						int offset { get_int_value(ptr + 1) };
						return Value { Heap_Ptr {
							offset >= 0 ? ram_begin + offset : nullptr
						} };
					}
					default: err(Err::unknown_type);
				}
			}

			template<signed char*& B, signed char*& E, Err::Code C>
			static void set_value(Ptr<B, E, C> ptr, const Value& value) {
				if (auto ch = std::get_if<signed char>(&value)) {
					ptr.check(ch_size);
					ptr.ptr_[0] = ch_type; ptr.ptr_[1] = *ch;
				} else if (auto val = std::get_if<int>(&value)) {
					ptr.check(int_size);
					ptr.ptr_[0] = int_type;
					set_int(ptr + 1, *val);
				} else if (auto pt = std::get_if<Heap_Ptr>(&value)) {
					ptr.check(ptr_size);
					ptr.ptr_[0] = ptr_type;
					int v = *pt ? static_cast<int>(pt->ptr_ - ram_begin) : -1;
					set_int(ptr + 1, v);
				} else { err(Err::unknown_type); }
			}

			template<typename P> static Heap_Ptr get_ptr(const P& ptr) {
				if (!ptr) { err(Err::null_access); }
				int value { get_int_value(ptr) };
				return Heap_Ptr { value >= 0 ? ram_begin + value : nullptr };
			}

			template<typename P> static void set_ptr(
				P ptr, const Heap_Ptr& value
			) {
				auto got { value.ptr_ };
				if (!ptr) { err(Err::null_access); }
				set_int(ptr, got ? static_cast<int>(got - ram_begin) : -1);
			}

			static List free_list;
			static List alloc_list;

			static Value pull();
			static signed char pull_ch();
			static int pull_int();
			static Heap_Ptr pull_ptr();

			static void push(Value value);
			static void insert_into_free_list(Heap_Ptr block);
	};
}