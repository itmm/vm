add_library(libvm
    accessor.cpp
    err.cpp
	heap.cpp
    list.cpp
    ptr.cpp
    value.cpp
    vm.cpp
	heap.cpp
	heap.h
)

add_subdirectory(ops)

target_include_directories(libvm INTERFACE .)

if("${Minimal}" STREQUAL "YES")
	target_compile_options(libvm PUBLIC
		-DCONFIG_WITH_INT=0 -DCONFIG_WITH_CHAR=0
		-DCONFIG_WITH_CALL=0 -DCONFIG_WITH_HEAP=0
	)
endif()