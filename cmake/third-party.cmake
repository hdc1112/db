# git clone https://github.com/fmtlib/fmt.git
# cd fmt; mkdir build; cd build; cmake ..; make; sudo make install
find_package(fmt REQUIRED)

# git clone git clone https://github.com/google/googletest
# cd googletest; mkdir build; cd build; cmake ..; make; sudo make install
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# git submodule add https://github.com/gabime/spdlog.git external/spdlog
add_subdirectory(external/spdlog)

# git submodule add https://github.com/ReactiveX/RxCpp.git external/RxCpp
add_subdirectory(external/RxCpp)

# Export a third-party libraries list
set(THIRD_PARTY_LIBS)
list(APPEND THIRD_PARTY_LIBS fmt::fmt ${GTEST_LIBRARIES} spdlog rxcpp)
