#include "ErrCode.hpp"
#include "FutureTestUtility.hpp"
#include "diskspace/DiskSpaceManager.hpp"
#include "diskspace/DiskSpaceTypes.hpp"
#include "utility/FileUtility.hpp"
#include "utility/ScopeGuard.hpp"
#include "utility/StopWatch.hpp"

#include "spdlog/spdlog.h"
#include "gtest/gtest.h"

#ifdef __APPLE__
#include <fcntl.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <vector>

namespace diskspace {
namespace {
using ::testing::Test;
using ::testing::TestWithParam;
using ::testing::Values;

class DoubleBufferingPerfTestFixture : public TestWithParam<BlockBytes> {
public:
    void SetUp() override {
        diskSpaceManager = new DiskSpaceManager();
        diskSpaceManager->start();
    }

    void TearDown() override {
        WAIT_FOR(diskSpaceManager->stop());
        delete diskSpaceManager;
        diskSpaceManager = nullptr;
    }

    DiskSpaceManager* diskSpaceManager = nullptr;
    constexpr static BlockNum blocks = 10;
};

TEST_P(DoubleBufferingPerfTestFixture, ControlGroup_SingleThreadComputeAndIO) {
    BlockBytes bytes = GetParam();

    auto* from = new uint8_t[bytes];
    auto* to = new uint8_t[bytes];
    std::fill(from, from + bytes, 0);

    char fromTmpFileName[]{"/tmp/from-tmpfile.GhLLaF"};
    WAIT_DISK_CMD(removeFile(diskSpaceManager, fromTmpFileName));
    WAIT_DISK_CMD_AND_ASSERT(createFile(diskSpaceManager, fromTmpFileName));

    char toTmpFileName[]{"/tmp/to-tmpfile.k1KukE"};
    WAIT_DISK_CMD(removeFile(diskSpaceManager, toTmpFileName));
    WAIT_DISK_CMD_AND_ASSERT(createFile(diskSpaceManager, toTmpFileName));

    for (int i = 0; i < blocks; ++i) {
        WAIT_DISK_CMD_AND_ASSERT(appendBlock(diskSpaceManager, fromTmpFileName, bytes, from));
    }

    utils::StopWatch stopWatch;
    stopWatch.start();
    for (int i = 0; i < blocks; ++i) {
        WAIT_DISK_CMD_AND_ASSERT(readBlock(diskSpaceManager, fromTmpFileName, i, bytes, from));
        for (int j = 0; j < bytes; ++j) {
            to[j] = from[j] + 1;
        }
        WAIT_DISK_CMD_AND_ASSERT(appendBlock(diskSpaceManager, toTmpFileName, bytes, to));
    }
    stopWatch.stop();
    SPDLOG_INFO("Single thread {} blocks {} bytes per block, takes time millis {}",
                blocks,
                bytes,
                stopWatch.elapsedMs().count());

    WAIT_DISK_CMD(removeFile(diskSpaceManager, fromTmpFileName));
    WAIT_DISK_CMD(removeFile(diskSpaceManager, toTmpFileName));
}

TEST_P(DoubleBufferingPerfTestFixture, TreatmentGroup_DoubleBuffering_CPU_IO_Parallel) {}

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
} // namespace
} // namespace diskspace