#include "diskspace/DiskSpaceManagerTest.hpp"
namespace diskspace {
std::unique_ptr<DiskSpaceManager> DiskSpaceManagerTest::diskSpaceManager = std::make_unique<DiskSpaceManager>();

TEST_F(DiskSpaceManagerTest, fileExisted) {
    char tmpFileName[]{"/tmp/db-tmpfile.XXXXXX"};
    mkstemp(tmpFileName);

    auto future = createFile(diskSpaceManager->borrow(), tmpFileName);
    WAIT_FOR(future);
    auto [success, errCode] = future.get();

    EXPECT_FALSE(success);
    EXPECT_EQ(ERR_FILE_EXISTED, errCode);

    unlink(tmpFileName);
}

TEST_F(DiskSpaceManagerTest, createNewFile) {
    char tmpFileName[]{"/tmp/db-tmpfile.kznwMh"};
    unlink(tmpFileName);

    auto future = createFile(diskSpaceManager->borrow(), tmpFileName);
    WAIT_FOR(future);
    auto [success, errCode] = future.get();

    EXPECT_TRUE(success);
    EXPECT_EQ(ERR_NO_ERROR, errCode);

    unlink(tmpFileName);
}
} // namespace diskspace