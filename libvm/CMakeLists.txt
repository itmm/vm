add_library(libvm
    accessor.cpp
    err.cpp
    list.cpp
    ptr.cpp
    value.cpp
    vm.cpp
        ops/poly.cpp
        ops/poly.h
)

add_subdirectory(ops)

target_include_directories(libvm INTERFACE .)