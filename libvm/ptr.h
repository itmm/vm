#pragma once

#include "err.h"

namespace vm {
	extern const signed char* old_code_begin;
	extern const signed char* old_code_end;
	extern signed char* old_ram_begin;
	#if CONFIG_WITH_HEAP
		extern signed char* old_heap_end;
	#endif
	extern signed char* old_stack_begin;
	#if CONFIG_WITH_CALL
		extern signed char* old_stack_end;
	#endif
	extern signed char* old_ram_end;

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

			void internal_check(int size, T begin, T end, Err::Code code) const;
	};

	class Code_Ptr: public Const_Ptr<
		const signed char*, old_code_begin, old_code_end, Err::leave_code_segment
	> {
		public:
			explicit Code_Ptr(const signed char* ptr = nullptr):
				Const_Ptr<const signed char*, old_code_begin, old_code_end, Err::leave_code_segment>(ptr) { }

			static const signed char* begin;
			static const signed char* end;

			void check(int size) const { internal_check(size, begin, end, Err::leave_code_segment); }
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

	class Ram_Ptr : public Ptr<old_ram_begin, old_ram_end, Err::leave_ram_segment> {
		public:
			explicit Ram_Ptr(signed char* ptr = nullptr): Ptr<old_ram_begin, old_ram_end, Err::leave_ram_segment>(ptr) { }

			static signed char* begin;
			static signed char* end;

			void check(int size) const { internal_check(size, begin, end, Err::leave_ram_segment); }
	};

	template<signed char*& B, signed char*& E, Err::Code C>
	class Casting_Ptr : public Ptr<B, E, C> {
		public:
			explicit Casting_Ptr(signed char* ptr = nullptr):
				Ptr<B, E, C> { ptr } { }

			explicit operator Ram_Ptr() const { return Ram_Ptr { this->ptr_ }; }
	};

	#if CONFIG_WITH_HEAP
		class Heap_Ptr : public Casting_Ptr<old_ram_begin, old_heap_end, Err::leave_heap_segment> {
			public:
				explicit Heap_Ptr(signed char* ptr = nullptr) : Casting_Ptr<old_ram_begin, old_heap_end, Err::leave_heap_segment>(ptr) { }

				static signed char* begin;
				static signed char* end;

				void check(int size) const { internal_check(size, begin, end, Err::leave_heap_segment); }
		};
	#endif

	#if CONFIG_WITH_CALL
		class Stack_Ptr : public Casting_Ptr<old_stack_begin, old_stack_end, Err::leave_stack_segment> {
			public:
				explicit Stack_Ptr(signed char* ptr = nullptr) : Casting_Ptr<old_stack_begin, old_stack_end, Err::leave_stack_segment>(ptr) { }

				static signed char* begin;
				static signed char* end;

				void check(int size) const { internal_check(size, begin, end, Err::leave_stack_segment); }
		};
	#else
		class Stack_Ptr : public Casting_Ptr<old_stack_begin, old_ram_end, Err::leave_stack_segment> {
			public:
				explicit Stack_Ptr(signed char* ptr = nullptr) : Casting_Ptr<old_stack_begin, old_ram_end, Err::leave_stack_segment>(ptr) { }

				static signed char* begin;
				static signed char* end;

				void check(int size) const { internal_check(size, begin, end, Err::leave_stack_segment); }
		};
	#endif

	inline signed char* stack_lower_limit() {
		#if CONFIG_WITH_HEAP
			return Heap_Ptr::end;
		#else
			return Ram_Ptr::begin;
		#endif
	}

	inline signed char* stack_upper_limit() {
		#if CONFIG_WITH_CALL
			return old_stack_end;
		#else
			return Ram_Ptr::end;
		#endif
	}

}