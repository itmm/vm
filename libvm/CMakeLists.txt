add_library(libvm
    accessor.cpp
    err.cpp
    list.cpp
    ptr.cpp
    value.cpp
    vm.cpp
)

target_include_directories(libvm INTERFACE .)