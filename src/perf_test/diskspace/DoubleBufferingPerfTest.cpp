#include "diskspace/DoubleBufferingPerfTest.hpp"

namespace diskspace {

std::unique_ptr<DiskSpaceManager> DoubleBufferingPerfTestFixture::diskSpaceManager =
    std::make_unique<DiskSpaceManager>();

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
        AssertAppendBlock(diskSpaceManager->borrow(), fromTmpFileName, bytes, from);
    }

    utils::StopWatch stopWatch;
    stopWatch.start();
    for (BlockId i = 0; i < blocks; ++i) {
        AssertReadBlock(diskSpaceManager->borrow(), fromTmpFileName, i, bytes, from);

        for (int j = 0; j < bytes; ++j) {
            to[j] = from[j] + 1;
        }

        AssertAppendBlock(diskSpaceManager->borrow(), toTmpFileName, bytes, to);
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
        AssertAppendBlock(diskSpaceManager->borrow(), fromTmpFileName, bytes, from0);
    }

    utils::StopWatch stopWatch;
    stopWatch.start();

    std::future<DiskCommandResult> fromFuture0 =
        readBlock(diskSpaceManager->borrow(), fromTmpFileName, 0, bytes, from0);
    std::future<DiskCommandResult> fromFuture1 = completedFuture<DiskCommandResult>();
    std::future<DiskCommandResult> toFuture0 = completedFuture<DiskCommandResult>();
    std::future<DiskCommandResult> toFuture1 = completedFuture<DiskCommandResult>();

    for (BlockId i = 0; i < blocks; ++i) {
        if (i % 2 == 0) {
            WAIT_FOR(fromFuture0);
            WAIT_FOR(toFuture0);
            ASSERT_TRUE(fromFuture0.get().success);
            ASSERT_TRUE(toFuture0.get().success);

            if (i + 1 < blocks) {
                fromFuture1 = readBlock(diskSpaceManager->borrow(), fromTmpFileName, i + 1, bytes, from1);
            }

            for (int j = 0; j < bytes; ++j) {
                to0[j] = from0[j] + 1;
            }

            toFuture0 = appendBlock(diskSpaceManager->borrow(), toTmpFileName, bytes, to0);
        } else {
            WAIT_FOR(fromFuture1);
            WAIT_FOR(toFuture1);
            ASSERT_TRUE(fromFuture1.get().success);
            ASSERT_TRUE(toFuture1.get().success);

            if (i + 1 < blocks) {
                fromFuture0 = readBlock(diskSpaceManager->borrow(), fromTmpFileName, i + 1, bytes, from0);
            }

            for (int j = 0; j < bytes; ++j) {
                to1[j] = from1[j] + 1;
            }

            toFuture1 = appendBlock(diskSpaceManager->borrow(), toTmpFileName, bytes, to1);
        }
    }

    if (fromFuture0.valid()) {
        WAIT_FOR(fromFuture0);
        ASSERT_TRUE(fromFuture0.get().success);
    }
    if (toFuture0.valid()) {
        WAIT_FOR(toFuture0);
        ASSERT_TRUE(toFuture0.get().success);
    }
    if (fromFuture1.valid()) {
        WAIT_FOR(fromFuture1);
        ASSERT_TRUE(fromFuture1.get().success);
    }
    if (toFuture1.valid()) {
        WAIT_FOR(toFuture1);
        ASSERT_TRUE(toFuture1.get().success);
    }

    stopWatch.stop();
    SPDLOG_INFO("Double Buffering {} blocks {} bytes per block, takes time millis {}",
                blocks,
                bytes,
                stopWatch.elapsedMs().count());
}

} // namespace diskspace