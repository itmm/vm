#pragma once

#include "err.h"

namespace vm {
	template<typename P> P operator+(const P& ptr, int offset);
	template<typename P> P operator-(const P& ptr, int offset);

	template<typename T> class Const_Ptr;

	template<typename T>
	bool operator==(const Const_Ptr<T>& a, const Const_Ptr<T>& b);

	template<typename T>
	bool operator<(const Const_Ptr<T>& a, const Const_Ptr<T>& b);

	template<typename T> class Const_Ptr {
		public:
			explicit Const_Ptr(T ptr = nullptr): ptr_ { ptr } { }

			explicit operator bool() const { return ptr_; }

		protected:
			friend class Acc;
			friend class Heap;
			template<typename P> friend P operator+(const P& ptr, int offset);
			template<typename P> friend P operator-(const P& ptr, int offset);

			friend bool operator==<>(
				const Const_Ptr<T>& a, const Const_Ptr<T>& b
			);
			friend bool operator< <>(
				const Const_Ptr<T>& a, const Const_Ptr<T>& b
			);

			T ptr_;

			void internal_check(int size, const signed char* begin, const signed char* end, Err::Code code) const;
			int internal_offset(const signed char* begin) const;
	};

	class Code_Ptr: public Const_Ptr<const signed char*> {
		public:
			explicit Code_Ptr(const signed char* ptr = nullptr): Const_Ptr<const signed char*>(ptr) { }

			static const signed char* begin;
			static const signed char* end;

			void check(int size) const { internal_check(size, begin, end, Err::leave_code_segment); }
			[[nodiscard]] int offset() const { return internal_offset(begin); }
	};

	extern Code_Ptr pc;

	class Ptr : public Const_Ptr<signed char*> {
		public:
			explicit Ptr(signed char* ptr = nullptr):
				Const_Ptr<signed char*> { ptr } { }

		protected:
			[[nodiscard]] signed char* ptr() const { return this->ptr_; }
	};

	class Ram_Ptr : public Ptr {
		public:
			Ram_Ptr() = delete;

			static signed char* begin;
			static signed char* end;
	};

	#if CONFIG_WITH_HEAP
		class Heap_Ptr : public Ptr {
			public:
				explicit Heap_Ptr(signed char* ptr = nullptr) : Ptr { ptr } { }

				static signed char* end;

				void check(int size) const { internal_check(size, Ram_Ptr::begin, end, Err::leave_heap_segment); }
				[[nodiscard]] int offset() const { return internal_offset(Ram_Ptr::begin); }
		};
	#endif

	#if CONFIG_WITH_CALL
		class Stack_Ptr : public Ptr {
			public:
				explicit Stack_Ptr(signed char* ptr = nullptr) : Ptr { ptr } { }

				static signed char* begin;
				static signed char* end;

				void check(int size) const { internal_check(size, begin, end, Err::leave_stack_segment); }
				[[nodiscard]] int offset() const { return internal_offset(Ram_Ptr::begin); }
		};

		class Temporarly_Increase_Stack_Size {
			public:
				explicit Temporarly_Increase_Stack_Size(int size) :
					old_stack_end { Stack_Ptr::end }
				{
					if (old_stack_end + size > Ram_Ptr::end) {
						err(Err::leave_stack_segment);
					}
					Stack_Ptr::end += size;
				}
				~Temporarly_Increase_Stack_Size() {
					Stack_Ptr::end = old_stack_end;
				}
			private:
				signed char* old_stack_end;
		};
	#else
		inline signed char* stack_lower_limit();

		class Stack_Ptr : public Ptr {
			public:
				explicit Stack_Ptr(signed char* ptr = nullptr) : Ptr { ptr } { }

				static signed char* begin;
				#if CONFIG_WITH_CALL
					static signed char* end;
				#endif

				void check(int size) const { internal_check(size, begin, stack_lower_limit(), Err::leave_stack_segment); }
				[[nodiscard]] int offset() const { return internal_offset(stack_lower_limit()); }
		};

		class Temporarly_Increase_Stack_Size {
			public:
				explicit Temporarly_Increase_Stack_Size(int) {
					err(Err::leave_stack_segment);
				}
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
			return Stack_Ptr::end;
		#else
			return Ram_Ptr::end;
		#endif
	}
}