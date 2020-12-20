cmake_minimum_required(VERSION 3.17)
project(db)

set(CMAKE_CXX_STANDARD 20)

include(cmake/third-party.cmake)

FILE(GLOB SRC_FILES src/main/**.cpp src/main/**.hpp)

FILE(GLOB UNIT_TEST_SRC_FILES src/test/**.cpp src/test/**.hpp)

add_executable(unit_test ${SRC_FILES} ${UNIT_TEST_SRC_FILES})
target_link_libraries(unit_test fmt::fmt ${GTEST_LIBRARIES})