#pragma once

#include "err.h"

namespace vm {
	extern const signed char* old_code_begin;
	extern const signed char* old_code_end;
	extern signed char* ram_begin;
	#if CONFIG_WITH_HEAP
		extern signed char* heap_end;
	#endif
	extern signed char* stack_begin;
	#if CONFIG_WITH_CALL
		extern signed char* stack_end;
	#endif
	extern signed char* ram_end;

	inline signed char* stack_lower_limit() {
		#if CONFIG_WITH_HEAP
			return heap_end;
		#else
			return ram_begin;
		#endif
	}

	inline signed char* stack_upper_limit() {
		#if CONFIG_WITH_CALL
			return stack_end;
		#else
			return ram_end;
		#endif
	}

	template<typename P> P operator+(const P& ptr, int offset);
	template<typename P> P operator-(const P& ptr, int offset);

	template<typename T, T& B, T& E, Err::Code C> class Const_Ptr;

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
				if (ptr_) { }
			}

			explicit operator bool() const { return ptr_; }

			[[nodiscard]] int offset() const { return ptr_ ? ptr_ - B : -1; }

		protected:
			friend class Acc;
			friend class Heap;
			template<typename P> friend P operator+(const P& ptr, int offset);
			template<typename P> friend P operator-(const P& ptr, int offset);

			friend bool operator==<>(
				const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
			);
			friend bool operator< <>(
				const Const_Ptr<T, B, E, C>& a, const Const_Ptr<T, B, E, C>& b
			);

			T ptr_;

			void check(int size) const;
	};

	class Code_Ptr: public Const_Ptr<
		const signed char*, old_code_begin, old_code_end, Err::leave_code_segment
	> {
		public:
			explicit Code_Ptr(const signed char* ptr = nullptr):
				Const_Ptr<const signed char*, old_code_begin, old_code_end, Err::leave_code_segment>(ptr) { }

			static const signed char* begin;
			static const signed char* end;
	};

	extern Code_Ptr pc;

	template<signed char*& B, signed char*& E, Err::Code C>
	class Ptr : public Const_Ptr<signed char*, B, E, C> {
		public:
			explicit Ptr(signed char* ptr = nullptr):
				Const_Ptr<signed char*, B, E, C> { ptr } { }

		protected:
			[[nodiscard]] signed char* ptr() const { return this->ptr_; }
	};

	class Ram_Ptr : public Ptr<ram_begin, ram_end, Err::leave_ram_segment> {
		public:
			explicit Ram_Ptr(signed char* ptr = nullptr): Ptr<ram_begin, ram_end, Err::leave_ram_segment>(ptr) { }

			static signed char* begin;
			static signed char* end;
	};

	template<signed char*& B, signed char*& E, Err::Code C>
	class Casting_Ptr : public Ptr<B, E, C> {
		public:
			explicit Casting_Ptr(signed char* ptr = nullptr):
				Ptr<B, E, C> { ptr } { }

			explicit operator Ram_Ptr() const { return Ram_Ptr { this->ptr_ }; }
	};

	#if CONFIG_WITH_HEAP
		class Heap_Ptr : public Casting_Ptr<ram_begin, heap_end, Err::leave_heap_segment> {
			public:
				Heap_Ptr(signed char* ptr = nullptr) : Casting_Ptr<ram_begin, heap_end, Err::leave_heap_segment>(ptr) { }

				static signed char* begin;
				static signed char* end;
		};
	#endif

	#if CONFIG_WITH_CALL
		class Stack_Ptr : public Casting_Ptr<stack_begin, stack_end, Err::leave_stack_segment> {
			public:
				Stack_Ptr(signed char* ptr = nullptr) : Casting_Ptr<stack_begin, stack_end, Err::leave_stack_segment>(ptr) { }

				static signed char* begin;
				static signed char* end;
		};
	#else
		class Stack_Ptr : public Casting_Ptr<stack_begin, ram_end, Err::leave_stack_segment> {
			public:
				Stack_Ptr(signed char* ptr = nullptr) : Casting_Ptr<stack_begin, ram_end, Err::leave_stack_segment>(ptr) { }

				static signed char* begin;
				static signed char* end;
		};
	#endif
}