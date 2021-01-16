# git clone https://github.com/fmtlib/fmt.git
# cd fmt; mkdir build; cd build; cmake ..; make; sudo make install
find_package(fmt REQUIRED)

# git clone git clone https://github.com/google/googletest
# cd googletest; mkdir build; cd build; cmake ..; make; sudo make install
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory(external/spdlog)

# Export a third-party libraries list
set(THIRD_PARTY_LIBS)
list(APPEND THIRD_PARTY_LIBS fmt::fmt ${GTEST_LIBRARIES} spdlog)
