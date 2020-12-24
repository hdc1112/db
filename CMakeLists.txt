cmake_minimum_required(VERSION 3.17)
project(db)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/flavor.cmake)
include(cmake/third-party.cmake)
include(cmake/utility.cmake)

configure_file(src/include/BuildFlagsGenerated.hpp.template generated-src/include/BuildFlagsGenerated.hpp)

include_directories(${CMAKE_BINARY_DIR}/generated-src/include/)
include_directories(./src/include)

file_glob_recurse(SRC_FILES "src/main/*.[ch]pp" )
file_glob_recurse(UNIT_TEST_SRC_FILES "src/test/*.[ch]pp")

add_executable(unit_test ${SRC_FILES} ${UNIT_TEST_SRC_FILES})
target_link_libraries(unit_test fmt::fmt ${GTEST_LIBRARIES} spdlog)
