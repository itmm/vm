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

	template<typename T, T& B, T& E> class Const_Ptr;

	template<typename T, T& B, T& E>
	bool operator==(
		const Const_Ptr<T, B, E>& a, const Const_Ptr<T, B, E>& b
	);

	template<typename T, T& B, T& E>
	bool operator<(
		const Const_Ptr<T, B, E>& a, const Const_Ptr<T, B, E>& b
	);

	template<typename T, T& B, T& E> class Const_Ptr {
		public:
			explicit Const_Ptr(T ptr = nullptr): ptr_ { ptr } {
				if (ptr_) { }
			}

			explicit operator bool() const { return ptr_; }

		protected:
			friend class Acc;
			friend class Heap;
			template<typename P> friend P operator+(const P& ptr, int offset);
			template<typename P> friend P operator-(const P& ptr, int offset);

			friend bool operator==<>(
				const Const_Ptr<T, B, E>& a, const Const_Ptr<T, B, E>& b
			);
			friend bool operator< <>(
				const Const_Ptr<T, B, E>& a, const Const_Ptr<T, B, E>& b
			);

			T ptr_;

			void internal_check(int size, const signed char* begin, const signed char* end, Err::Code code) const;
			int internal_offset(const signed char* begin) const;
	};

	class Code_Ptr: public Const_Ptr<
		const signed char*, old_code_begin, old_code_end
	> {
		public:
			explicit Code_Ptr(const signed char* ptr = nullptr):
				Const_Ptr<const signed char*, old_code_begin, old_code_end>(ptr) { }

			static const signed char* begin;
			static const signed char* end;

			void check(int size) const { internal_check(size, begin, end, Err::leave_code_segment); }
			[[nodiscard]] int offset() const { return internal_offset(begin); }
	};

	extern Code_Ptr pc;

	template<signed char*& B, signed char*& E>
	class Ptr : public Const_Ptr<signed char*, B, E> {
		public:
			explicit Ptr(signed char* ptr = nullptr):
				Const_Ptr<signed char*, B, E> { ptr } { }

		protected:
			[[nodiscard]] signed char* ptr() const { return this->ptr_; }
	};

	class Ram_Ptr : public Ptr<old_ram_begin, old_ram_end> {
		public:
			explicit Ram_Ptr(signed char* ptr = nullptr): Ptr<old_ram_begin, old_ram_end>(ptr) { }

			static signed char* begin;
			static signed char* end;

			void check(int size) const { internal_check(size, begin, end, Err::leave_ram_segment); }
			[[nodiscard]] int offset() const { return internal_offset(begin); }
	};

	template<signed char*& B, signed char*& E>
	class Casting_Ptr : public Ptr<B, E> {
		public:
			explicit Casting_Ptr(signed char* ptr = nullptr):
				Ptr<B, E> { ptr } { }

			explicit operator Ram_Ptr() const { return Ram_Ptr { this->ptr_ }; }
	};

	#if CONFIG_WITH_HEAP
		class Heap_Ptr : public Casting_Ptr<old_ram_begin, old_heap_end> {
			public:
				explicit Heap_Ptr(signed char* ptr = nullptr) : Casting_Ptr<old_ram_begin, old_heap_end>(ptr) { }

				static signed char* begin;
				static signed char* end;

				void check(int size) const { internal_check(size, begin, end, Err::leave_heap_segment); }
				[[nodiscard]] int offset() const { return internal_offset(begin); }
		};
	#endif

	#if CONFIG_WITH_CALL
		class Stack_Ptr : public Casting_Ptr<old_stack_begin, old_stack_end> {
			public:
				explicit Stack_Ptr(signed char* ptr = nullptr) : Casting_Ptr<old_stack_begin, old_stack_end>(ptr) { }

				static signed char* begin;
				static signed char* end;

				void check(int size) const { internal_check(size, begin, end, Err::leave_stack_segment); }
				[[nodiscard]] int offset() const { return internal_offset(begin); }
		};
	#else
		class Stack_Ptr : public Casting_Ptr<old_stack_begin, old_ram_end> {
			public:
				explicit Stack_Ptr(signed char* ptr = nullptr) : Casting_Ptr<old_stack_begin, old_ram_end>(ptr) { }

				static signed char* begin;
				#if CONFIG_WITH_CALL
					static signed char* end;
				#endif

				void check(int size) const { internal_check(size, begin, end, Err::leave_stack_segment); }
				[[nodiscard]] int offset() const { return internal_check(begin); }
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