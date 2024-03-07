add_library(libvm
    accessor.cpp
	balanced_tree.cpp
    err.cpp
	heap.cpp
	ordered_tree.cpp
    ptr.cpp
    value.cpp
    vm.cpp
)

add_subdirectory(ops)

target_include_directories(libvm INTERFACE .)

if("${Minimal}" STREQUAL "YES")
	target_compile_options(libvm PUBLIC
		-DCONFIG_WITH_INT=0 -DCONFIG_WITH_BYTE=0
		-DCONFIG_WITH_CALL=0 -DCONFIG_WITH_HEAP=0
	)
endif()