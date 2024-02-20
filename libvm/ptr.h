#pragma once

#include "err.h"

namespace vm {
	extern const signed char* code_begin;
	extern const signed char* code_end;
	extern signed char* ram_begin;
	extern signed char* heap_end;
	extern signed char* stack_begin;
	extern signed char* ram_end;

	class Accessor;

	template<typename T, T& B, T& E, Err::Code C> class Const_Ptr;

	template<typename T, T& B, T& E, Err::Code C>
	Const_Ptr<T, B, E, C> operator+(
		const Const_Ptr<T, B, E, C>& ptr, int offset
	);

	template<typename T, T& B, T& E, Err::Code C>
	bool operator==(
		const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
	);

	template<typename T, T& B, T& E, Err::Code C>
	bool operator<(
		const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
	);

	template<typename T, T& B, T& E, Err::Code C> class Const_Ptr {
		public:
			explicit Const_Ptr(T ptr = nullptr): ptr_ { ptr } {
				if (ptr_) { check(0); }
			}

			explicit operator bool() const { return ptr_; }

		protected:
			friend class Accessor;
			friend Const_Ptr<T, B, E, C> operator+<>(const Const_Ptr<T, B, E, C>& ptr, int offset);
			friend bool operator==<>(const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b);
			friend bool operator< <>(const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b);

			T ptr_;

			void check(int size) const;
	};

	using Code_Ptr = Const_Ptr<
		const signed char*, code_begin, code_end,
		Err::leave_code_segment
	>;

	extern Code_Ptr pc;

	template<signed char*& B, signed char*& E, Err::Code C> class Ptr;

	template<signed char*& B, signed char*& E, Err::Code C>
	inline Ptr<B, E, C> operator+(const Ptr<B, E, C>& ptr, int offset);

	template<signed char*& B, signed char*& E, Err::Code C>
	inline Ptr<B, E, C> operator-(const Ptr<B, E, C>& ptr, int offset);

	template<signed char*& B, signed char*& E, Err::Code C>
	class Ptr : public Const_Ptr<signed char*, B, E, C> {
		public:
			using value_type = Ptr<B, E, C>;

			friend value_type operator+<>(const value_type& ptr, int offset);
			friend value_type operator-<>(const value_type& ptr, int offset);

			explicit Ptr(signed char* ptr = nullptr):
				Const_Ptr<signed char*, B, E, C> { ptr } { }

		protected:
			[[nodiscard]] signed char* ptr() const { return this->ptr_; }
	};

	template<signed char*& B, signed char*& E, Err::Code C>
	inline Ptr<B, E, C> operator+(const Ptr<B, E, C>& ptr, int offset) {
		return Ptr<B, E, C>(ptr.ptr_ + offset);
	}

	template<signed char*& B, signed char*& E, Err::Code C>
	inline Ptr<B, E, C> operator-(const Ptr<B, E, C>& ptr, int offset) {
		return Ptr<B, E, C>(ptr.ptr_ - offset);
	}

	using Ram_Ptr = Ptr<ram_begin, ram_end, Err::leave_ram_segment>;

	template<signed char*& B, signed char*& E, Err::Code C> class Casting_Ptr;

	template<signed char*& B, signed char*& E, Err::Code C>
	inline Casting_Ptr<B, E, C> operator+(
		const Casting_Ptr<B, E, C>& ptr, int offset
	);

	template<signed char*& B, signed char*& E, Err::Code C>
	inline Casting_Ptr<B, E, C> operator-(
		const Casting_Ptr<B, E, C>& ptr, int offset
	);

	template<signed char*& B, signed char*& E, Err::Code C>
	class Casting_Ptr : public Ptr<B, E, C> {
		public:
			using value_type = Casting_Ptr<B, E, C>;

			friend value_type operator+<>(const value_type& ptr, int offset);
			friend value_type operator-<>(const value_type& ptr, int offset);

			explicit Casting_Ptr(signed char* ptr = nullptr):
				Ptr<B, E, C> { ptr } { }

			explicit operator Ram_Ptr() const { return Ram_Ptr { this->ptr_ }; }
	};

	template<signed char*& B, signed char*& E, Err::Code C>
	inline Casting_Ptr<B, E, C> operator+(
		const Casting_Ptr<B, E, C>& ptr, int offset
	) {
		return Casting_Ptr<B, E, C>(ptr.ptr_ + offset);
	}

	template<signed char*& B, signed char*& E, Err::Code C>
	inline Casting_Ptr<B, E, C> operator-(
		const Casting_Ptr<B, E, C>& ptr, int offset
	) { return Casting_Ptr<B, E, C>(ptr.ptr_ - offset); }

	using Heap_Ptr = Casting_Ptr<
		ram_begin, heap_end, Err::leave_heap_segment
	>;

	using Stack_Ptr = Casting_Ptr<
		stack_begin, ram_end, Err::leave_stack_segment
	>;
}