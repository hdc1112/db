#include "ErrCode.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include "spdlog/spdlog.h"
#include "gtest/gtest.h"

namespace diskspace {
namespace {
TEST(DiskSpaceManagerTest, fileExisted) {
    char tmpFileName[]{"/tmp/tmpfile.XXXXXX"};
    mkstemp(tmpFileName);
    bool created = createNewFile(tmpFileName);
    EXPECT_FALSE(created);
    EXPECT_EQ(ERR_FILE_EXISTED, errCode);
    if (!created) {
        spdlog::error("Cannot create new file {}, the error is {}", tmpFileName, strErrCode(errCode));
    }
    unlink(tmpFileName);
}
} // namespace
} // namespace diskspace