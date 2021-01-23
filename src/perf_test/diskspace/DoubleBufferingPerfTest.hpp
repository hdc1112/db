#include "FutureTestUtility.hpp"
#include "SizeTypes.hpp"
#include "diskspace/DiskSpaceManager.hpp"
#include "diskspace/DiskSpaceManagerTestUtility.hpp"
#include "diskspace/DiskSpaceTypes.hpp"
#include "utility/FileUtility.hpp"
#include "utility/ScopeGuard.hpp"
#include "utility/StopWatch.hpp"

#include "Log.hpp"
#include "gtest/gtest.h"

#ifdef __APPLE__
#include <fcntl.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <vector>

namespace diskspace {
using ::testing::Test;
using ::testing::TestWithParam;
using ::testing::Values;

// TODO: Need to tune disk time and cpu time to make double buffering scheme work
class DoubleBufferingPerfTestFixture : public TestWithParam<BlockBytes> {
public:
    static void SetUpTestCase() {
        diskSpaceManager->start();
    }

    static void TearDownTestCase() {
        auto future = diskSpaceManager->stop();
        WAIT_FOR(future);
    }

    void SetUp() override {
        RemoveFile(diskSpaceManager->borrow(), fromTmpFileName);
        AssertCreateFile(diskSpaceManager->borrow(), fromTmpFileName);

        RemoveFile(diskSpaceManager->borrow(), toTmpFileName);
        AssertCreateFile(diskSpaceManager->borrow(), toTmpFileName);
    }

    void TearDown() override {
        RemoveFile(diskSpaceManager->borrow(), fromTmpFileName);
        RemoveFile(diskSpaceManager->borrow(), toTmpFileName);
    }

    constexpr static BlockNum blocks = 10;

    constexpr static char fromTmpFileName[]{"/tmp/from-tmpfile.GhLLaF"};
    constexpr static char toTmpFileName[]{"/tmp/to-tmpfile.k1KukE"};

    static std::unique_ptr<DiskSpaceManager> diskSpaceManager;
};

INSTANTIATE_TEST_SUITE_P(DoubleBufferingPerfTest,
                         DoubleBufferingPerfTestFixture,
                         ::testing::Values(4_KB,
                                           8_KB,
                                           16_KB,
                                           32_KB,
                                           64_KB,
                                           128_KB,
                                           256_KB,
                                           512_KB,
                                           1_MB,
                                           2_MB,
                                           4_MB,
                                           8_MB,
                                           16_MB,
                                           32_MB,
                                           64_MB,
                                           128_MB,
                                           256_MB,
                                           512_MB));
} // namespace diskspace