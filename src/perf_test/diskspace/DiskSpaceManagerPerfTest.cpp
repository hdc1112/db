#include "ErrCode.hpp"
#include "diskspace/DiskSpaceManager.hpp"
#include "utility/FileUtility.hpp"
#include "utility/OperationUtility.hpp"
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

TEST(DiskSpaceManagerPerfTest, readDataFromMemory) {
    BlockNum blockNum = 1024;
    BlockBytes blockBytes = 1024 * 1024;
    auto* from = new uint8_t[blockNum * blockBytes];
    auto* to = new uint8_t[blockNum * blockBytes];

    utils::repeat(
        [from, to, blockNum, blockBytes]() {
            utils::StopWatch stopWatch;
            stopWatch.start();
            std::copy(from, from + blockNum * blockBytes, to);
            stopWatch.stop();
            SPDLOG_INFO("Read data from memory takes millis {}", stopWatch.elapsedMs().count());
        },
        10);
}

bool readFile(const char* fileName, BlockNum blockNum, BlockBytes blockBytes, bool withPageCache) {
    if (!utils::exists(fileName)) {
        errCode = ERR_NO_SUCH_FILE_OR_DIR;
        return false;
    }

    auto* buffer = new uint8_t[blockBytes];
    utils::ScopeGuard releaseBufferGuard([buffer]() { delete[] buffer; });

    errCode = ERR_NO_ERROR;

#ifdef __APPLE__
    SPDLOG_INFO("Take the Apple system code path to read a file");

    int oflag = O_RDONLY;
    int fd = open(fileName, oflag);
    if (fd == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    utils::ScopeGuard closeFdGuard([fd]() { close(fd); });

    int ret = fcntl(fd, F_GLOBAL_NOCACHE, withPageCache ? 0 : 1);
    if (ret == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    for (BlockNum i = 0; i < blockNum; ++i) {
        if (ret = read(fd, buffer, blockBytes); ret != blockBytes) {
            errCode = errnoToErrCode(errno);
            return false;
        }
    }

    return true;
#else
    SPDLOG_CRITICAL("Unsupported operating system");
    errCode = ERR_UNSUPPORTED_OS;
    return false;
#endif
}

TEST(DiskSpaceManagerPerfTest, readFile_WithPageCache) {
    char tmpFileName[]{"/tmp/db-tmpfile.kznwMh"};
    unlink(tmpFileName);

    BlockNum blockNum = 1024;
    BlockBytes blockBytes = 1024 * 1024;
    bool created = createNewFile(tmpFileName, blockNum, blockBytes);
    ASSERT_TRUE(created);

    utils::repeat([tmpFileName, blockNum, blockBytes]() { readFile(tmpFileName, blockNum, blockBytes, true); }, 10);

    utils::repeat(
        [tmpFileName, blockNum, blockBytes]() {
            utils::StopWatch stopWatch;
            stopWatch.start();
            readFile(tmpFileName, blockNum, blockBytes, true);
            stopWatch.stop();
            SPDLOG_INFO("Read file takes milli {} with page cache", stopWatch.elapsedMs().count());
        },
        10);

    unlink(tmpFileName);
}

TEST(DiskSpaceManagerPerfTest, readFile_WithoutPageCache) {
    char tmpFileName[]{"/tmp/db-tmpfile.kznwMh"};
    unlink(tmpFileName);

    BlockNum blockNum = 1024;
    BlockBytes blockBytes = 1024 * 1024;
    bool created = createNewFile(tmpFileName, blockNum, blockBytes);
    ASSERT_TRUE(created);

    utils::repeat([tmpFileName, blockNum, blockBytes]() { readFile(tmpFileName, blockNum, blockBytes, false); }, 10);

    utils::repeat(
        [tmpFileName, blockNum, blockBytes]() {
            utils::StopWatch stopWatch;
            stopWatch.start();
            readFile(tmpFileName, blockNum, blockBytes, false);
            stopWatch.stop();
            SPDLOG_INFO("Read file takes milli {} without page cache", stopWatch.elapsedMs().count());
        },
        10);

    unlink(tmpFileName);
}
} // namespace
} // namespace diskspace