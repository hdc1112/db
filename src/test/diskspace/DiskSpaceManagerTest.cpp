#include "ErrCode.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include "gtest/gtest.h"

#include <iostream>

namespace diskspace {
namespace {
TEST(DiskSpaceManagerTest, fileExisted) {
    char tmpFileName[]{"/tmp/tmpfile.XXXXXX"};
    mkstemp(tmpFileName);
    bool created = createNewFile(tmpFileName);
    EXPECT_FALSE(created);
    EXPECT_EQ(ERR_FILE_EXISTED, errCode);
    if (!created) {
        // TODO: add log level and log template here
        std::cout << strErrCode(errCode) << std::endl;
    }
    unlink(tmpFileName);
}
} // namespace
} // namespace diskspace