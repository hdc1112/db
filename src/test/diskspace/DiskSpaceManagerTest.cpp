#include "ErrCode.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include "gtest/gtest.h"

#include <iostream>

namespace diskspace {
namespace {
TEST(DiskSpaceManagerTest, fileExisted) {
    bool created = createNewFile("/tmp/arroyo-test.db.MnEEax");
    if (!created) {
        std::cout << errCode << std::endl;
    }
}
} // namespace
} // namespace diskspace