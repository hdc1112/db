#include "FutureTestUtility.hpp"
#include "diskspace/DiskSpaceManager.hpp"
#include "diskspace/DiskSpaceManagerTestUtility.hpp"
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

        RemoveFile(diskSpaceManager, fromTmpFileName);
        AssertCreateFile(diskSpaceManager, fromTmpFileName);

        RemoveFile(diskSpaceManager, toTmpFileName);
        AssertCreateFile(diskSpaceManager, toTmpFileName);
    }

    void TearDown() override {
        RemoveFile(diskSpaceManager, fromTmpFileName);
        RemoveFile(diskSpaceManager, toTmpFileName);

        WAIT_FOR(diskSpaceManager->stop());
        delete diskSpaceManager;
        diskSpaceManager = nullptr;
    }

    DiskSpaceManager* diskSpaceManager = nullptr;
    constexpr static BlockNum blocks = 10;

    constexpr static char fromTmpFileName[]{"/tmp/from-tmpfile.GhLLaF"};
    constexpr static char toTmpFileName[]{"/tmp/to-tmpfile.k1KukE"};
};

TEST_P(DoubleBufferingPerfTestFixture, ControlGroup_SingleThreadComputeAndIO) {
    BlockBytes bytes = GetParam();

    auto* from = new uint8_t[bytes];
    auto* to = new uint8_t[bytes];
    utils::ScopeGuard releaseBufferGuard([&from, &to]() {
        delete[] from;
        from = nullptr;
        delete[] to;
        to = nullptr;
    });

    std::fill(from, from + bytes, 0);
    for (BlockId i = 0; i < blocks; ++i) {
        AssertAppendBlock(diskSpaceManager, fromTmpFileName, bytes, from);
    }

    utils::StopWatch stopWatch;
    stopWatch.start();
    for (BlockId i = 0; i < blocks; ++i) {
        AssertReadBlock(diskSpaceManager, fromTmpFileName, i, bytes, from);

        for (int j = 0; j < bytes; ++j) {
            to[j] = from[j] + 1;
        }

        AssertAppendBlock(diskSpaceManager, toTmpFileName, bytes, to);
    }
    stopWatch.stop();
    SPDLOG_INFO("Single thread {} blocks {} bytes per block, takes time millis {}",
                blocks,
                bytes,
                stopWatch.elapsedMs().count());
}

TEST_P(DoubleBufferingPerfTestFixture, TreatmentGroup_DoubleBuffering_CPU_IO_Parallel) {
    BlockBytes bytes = GetParam();

    auto* from0 = new uint8_t[bytes];
    auto* to0 = new uint8_t[bytes];
    auto* from1 = new uint8_t[bytes];
    auto* to1 = new uint8_t[bytes];

    utils::ScopeGuard releaseBufferGuard([&from0, &to0, &from1, &to1]() {
        delete[] from0;
        from0 = nullptr;
        delete[] to0;
        to0 = nullptr;
        delete[] from1;
        from1 = nullptr;
        delete[] to1;
        to1 = nullptr;
    });

    std::fill(from0, from0 + bytes, 0);
    for (BlockId i = 0; i < blocks; ++i) {
        AssertAppendBlock(diskSpaceManager, fromTmpFileName, bytes, from0);
    }

    utils::StopWatch stopWatch;
    stopWatch.start();

    std::future<DiskCommandResult> fromFuture0 = readBlock(diskSpaceManager, fromTmpFileName, 0, bytes, from0);
    std::future<DiskCommandResult> fromFuture1;
    std::future<DiskCommandResult> toFuture0 = completedFuture<DiskCommandResult>();
    std::future<DiskCommandResult> toFuture1 = completedFuture<DiskCommandResult>();

    for (BlockId i = 0; i < blocks; ++i) {
        if (i % 2 == 0) {
            WAIT_FOR(fromFuture0);
            WAIT_FOR(toFuture0);

            if (i + 1 < blocks) {
                fromFuture1 = readBlock(diskSpaceManager, fromTmpFileName, i + 1, bytes, from1);
            }

            for (int j = 0; j < bytes; ++j) {
                to0[j] = from0[j] + 1;
            }

            toFuture0 = appendBlock(diskSpaceManager, toTmpFileName, bytes, to0);
        } else {
            WAIT_FOR(fromFuture1);
            WAIT_FOR(toFuture1);

            if (i + 1 < blocks) {
                fromFuture0 = readBlock(diskSpaceManager, fromTmpFileName, i + 1, bytes, from0);
            }

            for (int j = 0; j < bytes; ++j) {
                to1[j] = from1[j] + 1;
            }

            toFuture1 = appendBlock(diskSpaceManager, toTmpFileName, bytes, to1);
        }
    }
    stopWatch.stop();
    SPDLOG_INFO("Double Buffering {} blocks {} bytes per block, takes time millis {}",
                blocks,
                bytes,
                stopWatch.elapsedMs().count());
}

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