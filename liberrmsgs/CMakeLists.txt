add_library(liberrmsgs errmsgs.cpp)

target_include_directories(liberrmsgs INTERFACE .)
target_link_libraries(liberrmsgs libvm)