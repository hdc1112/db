#include "diskspace/DiskSpaceManager.hpp"
#include "ErrCode.hpp"
#include "utility/FileUtility.hpp"
#include "utility/ScopeGuard.hpp"

#include "spdlog/spdlog.h"

#ifdef __APPLE__
#include <fcntl.h>
#include <unistd.h>
#endif

#include <vector>

namespace diskspace {
bool createNewFile(const char* fileName, BlockNum blockNum, BlockBytes blockBytes) {
    if (utils::exists(fileName)) {
        errCode = ERR_FILE_EXISTED;
        return false;
    }

    std::vector<uint8_t> block(blockBytes, 0);

    errCode = ERR_NO_ERROR;

#ifdef __APPLE__
    spdlog::info("Take the Apple system code path to create a new file");

    int fd = open(fileName, O_CREAT | O_WRONLY);
    if (fd == -1) {
        errCode = ERR_UNCATEGORIZED;
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
        errCode = ERR_UNCATEGORIZED;
        return false;
    }

    for (BlockNum i = 0; i < blockNum; ++i) {
        if (ret = write(fd, block.data(), blockBytes); ret != blockBytes) {
            errCode = ERR_UNCATEGORIZED;
            return false;
        }
    }

    return true;
#else
    spdlog::critical("Unsupported operating system");
    errCode = ERR_UNSUPPORTED_OS;
    return false;
#endif
}
} // namespace diskspace