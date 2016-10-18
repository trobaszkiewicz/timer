cmake_minimum_required (VERSION 2.6)

include_directories("${CMAKE_SOURCE_DIR}")
add_definitions("-std=c++11")

add_executable(simple_timer_usage simple_timer_usage.cpp)
target_link_libraries(simple_timer_usage pthread)
