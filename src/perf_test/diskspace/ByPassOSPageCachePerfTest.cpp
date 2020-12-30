#include "ErrCode.hpp"
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

bool createNewFile(const char* fileName, BlockNum blockNum, BlockBytes blockBytes) {
    if (utils::exists(fileName)) {
        errCode = ERR_FILE_EXISTED;
        return false;
    }

    std::vector<uint8_t> block(blockBytes, 0);

    errCode = ERR_NO_ERROR;

#ifdef __APPLE__
    SPDLOG_INFO("Take the Apple system code path to create a new file");

    int oflag = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    int fd = open(fileName, oflag, mode);
    if (fd == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    utils::ScopeGuard closeFdGuard([fd, fileName]() {
        close(fd);

        if (errCode != ERR_NO_ERROR) {
            spdlog::error("Delete the file {} since error happened", fileName);
            unlink(fileName);
        }
    });

    int ret = fcntl(fd, F_GLOBAL_NOCACHE, 1);
    if (ret == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    for (BlockNum i = 0; i < blockNum; ++i) {
        if (ret = write(fd, block.data(), blockBytes); ret != blockBytes) {
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

bool readFile(const char* fileName, BlockNum blockNum, BlockBytes blockBytes, bool withPageCache) {
    if (!utils::exists(fileName)) {
        errCode = ERR_NO_SUCH_FILE_OR_DIR;
        return false;
    }

    auto* buffer = new uint8_t[blockBytes];
    utils::ScopeGuard releaseBufferGuard([buffer]() { delete[] buffer; });

    errCode = ERR_NO_ERROR;

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
}

TEST(ByPassOSPageCachePerfTest, readDataFromMemory) {
    BlockNum blockNum = 1024;
    BlockBytes blockBytes = 1024 * 1024;
    auto* from = new uint8_t[blockNum * blockBytes];
    auto* to = new uint8_t[blockNum * blockBytes];

    for (int i = 0; i < 10; ++i) {
        utils::StopWatch stopWatch;
        stopWatch.start();
        std::copy(from, from + blockNum * blockBytes, to);
        stopWatch.stop();
        SPDLOG_INFO("Read data from memory takes millis {}", stopWatch.elapsedMs().count());
    }
}

TEST(ByPassOSPageCachePerfTest, readFile_WithPageCache) {
    char tmpFileName[]{"/tmp/db-tmpfile.kznwMh"};
    unlink(tmpFileName);

    BlockNum blockNum = 1024;
    BlockBytes blockBytes = 1024 * 1024;
    bool created = createNewFile(tmpFileName, blockNum, blockBytes);
    ASSERT_TRUE(created);

    // warm up the os page cache
    for (int i = 0; i < 10; ++i) {
        readFile(tmpFileName, blockNum, blockBytes, true);
    }

    for (int i = 0; i < 10; ++i) {
        utils::StopWatch stopWatch;
        stopWatch.start();
        readFile(tmpFileName, blockNum, blockBytes, true);
        stopWatch.stop();
        SPDLOG_INFO("Read file takes milli {} with page cache", stopWatch.elapsedMs().count());
    }

    unlink(tmpFileName);
}

TEST(ByPassOSPageCachePerfTest, readFile_WithoutPageCache) {
    char tmpFileName[]{"/tmp/db-tmpfile.kznwMh"};
    unlink(tmpFileName);

    BlockNum blockNum = 1024;
    BlockBytes blockBytes = 1024 * 1024;
    bool created = createNewFile(tmpFileName, blockNum, blockBytes);
    ASSERT_TRUE(created);

    // exclude the factor of warm os page cache
    for (int i = 0; i < 10; ++i) {
        readFile(tmpFileName, blockNum, blockBytes, false);
    }

    for (int i = 0; i < 10; ++i) {
        utils::StopWatch stopWatch;
        stopWatch.start();
        readFile(tmpFileName, blockNum, blockBytes, false);
        stopWatch.stop();
        SPDLOG_INFO("Read file takes milli {} without page cache", stopWatch.elapsedMs().count());
    }

    unlink(tmpFileName);
}
} // namespace
} // namespace diskspace