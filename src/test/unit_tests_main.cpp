#include "utility/BuildUtility.hpp"

#include "spdlog/spdlog.h"
#include "gtest/gtest.h"

GTEST_API_ int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::trace);

    utils::printBuildFlags();

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
