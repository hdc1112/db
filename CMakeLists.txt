cmake_minimum_required(VERSION 3.17)
project(db)

set(CMAKE_CXX_STANDARD 20)

include(cmake/third-party.cmake)

enable_testing()

add_executable(unit_tests
        src/main/Page.hpp
        src/test/unit_tests_main.cpp)
target_link_libraries(unit_tests fmt::fmt ${GTEST_LIBRARIES})