#include "ErrCode.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include "gtest/gtest.h"

#include "FutureTestUtility.hpp"

namespace diskspace {
using ::testing::Test;

class DiskSpaceManagerTest : public Test {
public:
    static void SetUpTestCase() {
        diskSpaceManager->start();
    }

    static void TearDownTestCase() {
        auto future = diskSpaceManager->stop();
        WAIT_FOR(future);
    }

    static std::unique_ptr<DiskSpaceManager> diskSpaceManager;
};
} // namespace diskspace