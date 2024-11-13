cmake_minimum_required(VERSION 3.29)

project(units)
add_subdirectory(lib/catch2)
add_executable(ring_units ring_buffer_test.cpp)
target_link_libraries(ring_units PRIVATE Catch2WithMain)
include(CTest)
include(Catch)
catch_discover_tests(ring_units

  TEST_SPEC *
)
