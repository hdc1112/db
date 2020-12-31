#include "ErrCode.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include "gtest/gtest.h"

#include "FutureTestUtility.hpp"

namespace diskspace {
namespace {

using ::testing::Test;

class DiskSpaceManagerTest : public Test {
public:
    void SetUp() override {
        diskSpaceManager = new DiskSpaceManager();
        diskSpaceManager->start();
    }

    void TearDown() override {
        auto future = diskSpaceManager->stop();
        WAIT_FOR(future);
        delete diskSpaceManager;
        diskSpaceManager = nullptr;
    }

    DiskSpaceManager* diskSpaceManager = nullptr;
};

TEST_F(DiskSpaceManagerTest, fileExisted) {
    char tmpFileName[]{"/tmp/db-tmpfile.XXXXXX"};
    mkstemp(tmpFileName);

    auto future = createFile(diskSpaceManager, tmpFileName);
    WAIT_FOR(future);
    auto [success, errCode] = future.get();

    EXPECT_FALSE(success);
    EXPECT_EQ(ERR_FILE_EXISTED, errCode);

    unlink(tmpFileName);
}

TEST_F(DiskSpaceManagerTest, createNewFile) {
    char tmpFileName[]{"/tmp/db-tmpfile.kznwMh"};
    unlink(tmpFileName);

    auto future = createFile(diskSpaceManager, tmpFileName);
    WAIT_FOR(future);
    auto [success, errCode] = future.get();

    EXPECT_TRUE(success);
    EXPECT_EQ(ERR_NO_ERROR, errCode);

    unlink(tmpFileName);
}
} // namespace
} // namespace diskspace