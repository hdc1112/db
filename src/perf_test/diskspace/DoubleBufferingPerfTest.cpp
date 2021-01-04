#include "diskspace/DoubleBufferingPerfTest.hpp"
#include "utility/FutureUtility.hpp"

namespace diskspace {

std::unique_ptr<DiskSpaceManager> DoubleBufferingPerfTestFixture::diskSpaceManager =
    std::make_unique<DiskSpaceManager>();

TEST_P(DoubleBufferingPerfTestFixture, ControlGroup_SingleThreadComputeAndIO) {
    BlockBytes bytes = GetParam();

    std::vector<uint8_t> from(bytes);
    std::fill(std::begin(from), std::end(from), 0);
    for (BlockId i = 0; i < blocks; ++i) {
        AssertAppendBlock(diskSpaceManager->borrow(), fromTmpFileName, bytes, from.data());
    }

    std::vector<uint8_t> to(bytes);

    utils::StopWatch stopWatch;
    stopWatch.start();
    for (BlockId i = 0; i < blocks; ++i) {
        AssertReadBlock(diskSpaceManager->borrow(), fromTmpFileName, i, bytes, from.data());
        std::transform(std::begin(from), std::end(from), std::begin(to), [](uint8_t e) { return e + 1; });
        AssertAppendBlock(diskSpaceManager->borrow(), toTmpFileName, bytes, to.data());
    }
    stopWatch.stop();
    SPDLOG_INFO("Single thread {} blocks {} bytes per block, takes time millis {}",
                blocks,
                bytes,
                stopWatch.elapsedMs().count());
}

TEST_P(DoubleBufferingPerfTestFixture, TreatmentGroup_DoubleBuffering_CPU_IO_Parallel) {
    BlockBytes bytes = GetParam();

    std::vector<uint8_t> from0(bytes);
    std::vector<uint8_t> from1(bytes);
    std::vector<uint8_t> to0(bytes);
    std::vector<uint8_t> to1(bytes);

    std::fill(std::begin(from0), std::end(from0), 0);
    for (BlockId i = 0; i < blocks; ++i) {
        AssertAppendBlock(diskSpaceManager->borrow(), fromTmpFileName, bytes, from0.data());
    }

    utils::StopWatch stopWatch;
    stopWatch.start();

    std::shared_future<DiskCommandResult> fromFuture0 =
        readBlock(diskSpaceManager->borrow(), fromTmpFileName, 0, bytes, from0.data());
    std::shared_future<DiskCommandResult> fromFuture1 = utils::completedFuture<DiskCommandResult>();
    std::shared_future<DiskCommandResult> toFuture0 = utils::completedFuture<DiskCommandResult>();
    std::shared_future<DiskCommandResult> toFuture1 = utils::completedFuture<DiskCommandResult>();

    for (BlockId i = 0; i < blocks; ++i) {
        if (i % 2 == 0) {
            WAIT_FOR(fromFuture0);
            WAIT_FOR(toFuture0);
            ASSERT_TRUE(fromFuture0.get().success);
            ASSERT_TRUE(toFuture0.get().success);

            if (i + 1 < blocks) {
                fromFuture1 = readBlock(diskSpaceManager->borrow(), fromTmpFileName, i + 1, bytes, from1.data());
            }
            std::transform(std::begin(from0), std::end(from0), std::begin(to0), [](uint8_t e) { return e + 1; });
            toFuture0 = appendBlock(diskSpaceManager->borrow(), toTmpFileName, bytes, to0.data());
        } else {
            WAIT_FOR(fromFuture1);
            WAIT_FOR(toFuture1);
            ASSERT_TRUE(fromFuture1.get().success);
            ASSERT_TRUE(toFuture1.get().success);

            if (i + 1 < blocks) {
                fromFuture0 = readBlock(diskSpaceManager->borrow(), fromTmpFileName, i + 1, bytes, from0.data());
            }
            std::transform(std::begin(from1), std::end(from1), std::begin(to1), [](uint8_t e) { return e + 1; });
            toFuture1 = appendBlock(diskSpaceManager->borrow(), toTmpFileName, bytes, to1.data());
        }
    }

    WAIT_FOR(fromFuture0);
    ASSERT_TRUE(fromFuture0.get().success);
    WAIT_FOR(toFuture0);
    ASSERT_TRUE(toFuture0.get().success);
    WAIT_FOR(fromFuture1);
    ASSERT_TRUE(fromFuture1.get().success);
    WAIT_FOR(toFuture1);
    ASSERT_TRUE(toFuture1.get().success);

    stopWatch.stop();
    SPDLOG_INFO("Double Buffering {} blocks {} bytes per block, takes time millis {}",
                blocks,
                bytes,
                stopWatch.elapsedMs().count());
}

} // namespace diskspace