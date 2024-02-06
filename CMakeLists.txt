cmake_minimum_required(VERSION 3.27)
project(vm)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libvm)
add_subdirectory(liberrmsgs)
add_subdirectory(tests)

add_executable(vm main.cpp)
target_link_libraries(vm libvm liberrmsgs)