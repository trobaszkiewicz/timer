cmake_minimum_required (VERSION 2.6)

include_directories("${CMAKE_SOURCE_DIR}")
add_definitions("-std=c++11")

add_executable(test_timer test_timer.cpp)
target_link_libraries(test_timer gmock gmock_main pthread)
