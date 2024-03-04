#pragma once

#if !defined(CONFIG_INTERNAL_INT_TYPE)
	#define CONFIG_INTERNAL_INT_TYPE int
	#define CONFIG_INTERNAL_INT_TYPE_IS_STD_INT true
#endif

#if !defined(CONFIG_INTERNAL_INT_TYPE_IS_STD_INT)
	#define  CONFIG_INTERNAL_INT_TYPE_IS_STD_INT false
#endif

#if !defined(CONFIG_WITH_INT)
	#define CONFIG_WITH_INT true
#endif

#if CONFIG_WITH_INT
	#if !defined(CONFIG_INT_SIZE)
		#define CONFIG_INT_SIZE 4
	#endif
	#if !defined(CONFIG_INT_TYPE)
		#define CONFIG_INT_TYPE int
		#define CONFIG_INT_TYPE_IS_STD_INT true
	#endif
	#if !defined(CONFIG_INT_USE_TYPE_FIELD)
		#define CONFIG_INT_USE_TYPE_FIELD false
	#endif
	#if !defined(CONFIG_INT_MULTI_PRECISION)
		#define CONFIG_INT_MULTI_PRECISION false
	#endif
#endif

#if !defined(CONFIG_INT_TYPE_IS_STD_INT)
	#define CONFIG_INT_TYPE_IS_STD_INT false
#endif

#if !defined(CONFIG_WITH_CHAR)
	#define CONFIG_WITH_CHAR true
#endif

#if CONFIG_WITH_CHAR
	#if !defined(CONFIG_CHAR_SIZE)
		#define CONFIG_CHAR_SIZE 1
	#endif
	#if !defined(CONFIG_CHAR_TYPE)
		#define CONFIG_CHAR_TYPE signed char
	#endif
	#if !defined(CONFIG_CHAR_USE_TYPE_FIELD)
		#define CONFIG_CHAR_USE_TYPE_FIELD false
	#endif
#endif

#if !defined(CONFIG_WITH_CALL)
	#define CONFIG_WITH_CALL true
#endif

#if CONFIG_WITH_CALL
	#if !CONFIG_WITH_INT
		#error "call needs int"
	#endif
	#if !defined(CONFIG_WITH_EXCEPTIONS)
		#define CONFIG_WITH_EXCEPTIONS true
	#endif
#endif

#if !defined(CONFIG_WITH_HEAP)
	#define CONFIG_WITH_HEAP true
#endif

#if CONFIG_WITH_HEAP
	#if !CONFIG_WITH_INT
		#error "heap needs int"
	#endif

	#if !defined(CONFIG_HEAP_PTR_SIZE)
		#define CONFIG_HEAP_PTR_SIZE CONFIG_INT_SIZE
	#endif
	#if !defined(CONFIG_HEAP_PTR_USE_TYPE_FIELD)
		#define CONFIG_HEAP_PTR_USE_TYPE_FIELD CONFIG_INT_USE_TYPE_FIELD
	#endif
	#if !defined(CONFIG_WITH_GC)
		#define CONFIG_WITH_GC true
	#endif
#endif

#if !defined(CONFIG_STACK_PTR_SIZE)
	#define CONFIG_STACK_PTR_SIZE CONFIG_INT_SIZE
#endif
#if !defined(CONFIG_STACK_PTR_USE_TYPE_FIELD)
	#define CONFIG_STACK_PTR_USE_TYPE_FIELD CONFIG_INT_USE_TYPE_FIELD
#endif

#if !defined(CONFIG_INTERNAL_INT_IS_INT)
	#if CONFIG_INTERNAL_INT_TYPE_IS_STD_INT && CONFIG_INT_TYPE_IS_STD_INT
		#define CONFIG_INTERNAL_INT_IS_INT true
	#else
		#define CONFIG_INTERNAL_INT_IS_INT false
	#endif
#endif
#define CONFIG_WITH_NUMERIC (CONFIG_WITH_INT || CONFIG_WITH_CHAR)