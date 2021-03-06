cmake_minimum_required(VERSION 3.16)
project(db)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/macos.cmake)
include(cmake/flavor.cmake)
include(cmake/third-party.cmake)
include(cmake/utility.cmake)

configure_file(src/include/BuildFlagsGenerated.hpp.template generated-src/include/BuildFlagsGenerated.hpp)

include_directories(${CMAKE_BINARY_DIR}/generated-src/include/)
include_directories(./src/include)

file_glob_recurse(SRC_FILES "src/main/*.[ch]pp" )
file_glob_recurse(TEST_COMMON_SRC_FILES "src/test_common/*.[ch]pp")
file_glob_recurse(UNIT_TEST_SRC_FILES "src/unit_test/*.[ch]pp")
file_glob_recurse(PERF_TEST_SRC_FILES "src/perf_test/*.[ch]pp")

add_executable(unit_test ${SRC_FILES} ${UNIT_TEST_SRC_FILES} ${TEST_COMMON_SRC_FILES})
target_include_directories(unit_test PUBLIC src/unit_test src/test_common)
target_link_libraries(unit_test ${THIRD_PARTY_LIBS})

add_executable(perf_test ${SRC_FILES} ${PERF_TEST_SRC_FILES} ${TEST_COMMON_SRC_FILES})
target_include_directories(perf_test PUBLIC src/perf_test src/test_common)
target_link_libraries(perf_test ${THIRD_PARTY_LIBS})
