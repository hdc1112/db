#include "diskspace/DiskSpaceManager.hpp"
#include "Assert.hpp"
#include "ErrCode.hpp"
#include "utility/FileUtility.hpp"
#include "utility/ScopeGuard.hpp"

#include "spdlog/spdlog.h"

#ifdef __APPLE__
#include <fcntl.h>
#include <unistd.h>
#endif

#include <variant>
#include <vector>

namespace diskspace {
bool processCreateFileDiskCommand(const CreateFileCommand& createFileCommand) {
    SPDLOG_INFO("Processing create file command {}", createFileCommand);

    const char* fileName = createFileCommand.fileName;
    if (utils::exists(fileName)) {
        errCode = ERR_FILE_EXISTED;
        return false;
    }

    int oflag = O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    int fd = open(fileName, oflag, mode);
    if (fd == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    if (int ret = close(fd); ret == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    return true;
}

bool processRemoveFileDiskCommand(const RemoveFileCommand& removeFileCommand) {
    SPDLOG_INFO("Processing remove file command {}", removeFileCommand);

    const char* fileName = removeFileCommand.fileName;
    if (int ret = unlink(fileName); ret == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }
    return true;
}

bool processAppendBlockCommand(const AppendBlockCommand& appendBlockCommand) {
    SPDLOG_INFO("Processing append block command {}", appendBlockCommand);

    auto [fileName, bytes, from] = appendBlockCommand;

    int oflag = O_APPEND;
    int fd = open(fileName, oflag);
    if (fd == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    [[maybe_unused]] utils::ScopeGuard closeFdGuard([fd]() {
        if (int ret = close(fd); ret == -1) {
            errCode = errnoToErrCode(errno);
        }
    });

    if (int ret = fcntl(fd, F_GLOBAL_NOCACHE, 1); ret == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    if (int ret = write(fd, from, bytes); ret != bytes) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    return true;
}

bool processWriteBlockCommand(const WriteBlockCommand& writeBlockCommand) {
    SPDLOG_INFO("Processing write block command {}", writeBlockCommand);

    auto [fileName, blockId, bytes, from] = writeBlockCommand;

    int oflag = O_WRONLY;
    int fd = open(fileName, oflag);
    if (fd == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    [[maybe_unused]] utils::ScopeGuard closeFdGuard([fd]() {
        if (int ret = close(fd); ret == -1) {
            errCode = errnoToErrCode(errno);
        }
    });

    if (int ret = fcntl(fd, F_GLOBAL_NOCACHE, 1); ret == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    if (int ret = lseek(fd, blockId * bytes, SEEK_SET); ret == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    if (int ret = write(fd, from, bytes); ret != bytes) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    return true;
}

bool processReadBlockCommand(const ReadBlockCommannd& readBlockCommannd) {
    SPDLOG_INFO("Processing read block command {}", readBlockCommannd);

    auto [fileName, blockId, bytes, to] = readBlockCommannd;

    int oflag = O_RDONLY;
    int fd = open(fileName, oflag);
    if (fd == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    [[maybe_unused]] utils::ScopeGuard closeFdGuard([fd]() {
        if (int ret = close(fd); ret == -1) {
            errCode = errnoToErrCode(errno);
        }
    });

    if (int ret = fcntl(fd, F_GLOBAL_NOCACHE, 1); ret == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    if (int ret = lseek(fd, blockId * bytes, SEEK_SET); ret == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    if (int ret = read(fd, to, bytes); ret != bytes) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    return true;
}

bool processDiskCommand(const DiskCommand& diskCommand) {
    return std::visit(
        [](auto&& argument) -> bool {
            using T = std::decay_t<decltype(argument)>;
            if constexpr (std::is_same_v<T, CreateFileCommand>) {
                return processCreateFileDiskCommand(CreateFileCommand(argument));
            } else if constexpr (std::is_same_v<T, RemoveFileCommand>) {
                return processRemoveFileDiskCommand(RemoveFileCommand(argument));
            } else if constexpr (std::is_same_v<T, AppendBlockCommand>) {
                return processAppendBlockCommand(AppendBlockCommand(argument));
            } else if constexpr (std::is_same_v<T, WriteBlockCommand>) {
                return processWriteBlockCommand(WriteBlockCommand(argument));
            } else if constexpr (std::is_same_v<T, ReadBlockCommannd>) {
                return processReadBlockCommand(ReadBlockCommannd(argument));
            } else {
                errCode = ERR_ILLEGAL_ARG;
                return false;
            }
        },
        diskCommand);
}

void DiskSpaceManager::start() {
    transferState(DiskIOThreadState::Open, DiskIOThreadState::Running);
    _thread = std::thread(&DiskSpaceManager::run, this);
    _thread.detach();
    SPDLOG_INFO("DiskSpaceManager started an IO thread {}", _thread.get_id());
}

std::future<void> DiskSpaceManager::stop() {
    SPDLOG_INFO("DiskSpaceManager is shutting down ...");
    transferState(DiskIOThreadState::Running, DiskIOThreadState::Stopping);
    return _stopPromise.get_future();
}

void DiskSpaceManager::transferState(DiskIOThreadState expectCurrentState, DiskIOThreadState newState) {
    DEBUG_ASSERT(_state == expectCurrentState, "Current actual state {}, expected state{}", _state, expectCurrentState);
    SPDLOG_INFO("Disk thread state updated from {} to {}", _state, newState);
    _state = newState;
}

std::future<DiskCommandReport> DiskSpaceManager::submit(const DiskCommand& diskCommand) {
    std::promise<DiskCommandReport> reportPromise;
    auto reportFuture = reportPromise.get_future();
    submit(DiskCommandsGroup({diskCommand}, std::move(reportPromise)));
    return reportFuture;
}

std::future<DiskCommandReport> DiskSpaceManager::submit(const std::vector<DiskCommand>& diskCommands) {
    std::promise<DiskCommandReport> reportPromise;
    auto reportFuture = reportPromise.get_future();
    submit(DiskCommandsGroup(diskCommands, std::move(reportPromise)));
    return reportFuture;
}

void DiskSpaceManager::submit(DiskCommandsGroup diskCommandsGroup) {
    if (_state != DiskIOThreadState::Running) {
        diskCommandsGroup.reportPromise.set_value({false, ERR_SERVICE_UNAVAILABLE});
        return;
    }

    _q.enqueue(std::move(diskCommandsGroup));
}

void DiskSpaceManager::run() {
    while (_state == DiskIOThreadState::Running || _state == DiskIOThreadState::Stopping) {
        auto diskCommandsGroup = _q.tryDequeue(dequeueTimeout);

        if (!diskCommandsGroup) {
            if (_state == DiskIOThreadState::Running) {
                continue;
            } else {
                break;
            }
        }

        bool commandsSuccess = true;
        ErrCode commandsErrCode = ERR_NO_ERROR;
        for (auto diskCommand : diskCommandsGroup->diskCommands) {
            if (bool success = processDiskCommand(diskCommand); !success) {
                commandsSuccess = false;
                commandsErrCode = errCode;
                break;
            }
        }
        diskCommandsGroup->reportPromise.set_value({commandsSuccess, commandsErrCode});
    }
    transferState(DiskIOThreadState::Stopping, DiskIOThreadState::Stopped);
    _stopPromise.set_value();
}

} // namespace diskspace