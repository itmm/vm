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