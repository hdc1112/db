
find_package(fmt REQUIRED)

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory(external/spdlog)
find_package(spdlog)


# Export a third-party libraries list
set(THIRD_PARTY_LIBS)
list(APPEND THIRD_PARTY_LIBS fmt::fmt ${GTEST_LIBRARIES} spdlog)