#include "diskspace/DiskSpaceManager.hpp"
#include "Assert.hpp"
#include "ErrCode.hpp"
#include "utility/FileUtility.hpp"
#include "utility/ScopeGuard.hpp"

#include "Log.hpp"

#ifdef __APPLE__
#include <fcntl.h>
#include <unistd.h>
#endif

#include <variant>
#include <vector>

namespace diskspace {
bool processCreateFileDiskCommand(const CreateFileCommand& createFileCommand) {
    const char* fileName = createFileCommand.fileName;
    if (utils::exists(fileName)) {
        errCode = ERR_FILE_EXISTED;
        return false;
    }

    int oflag = O_CREAT | O_RDWR | O_TRUNC | O_EXCL;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    int fd = open(fileName, oflag, mode);
    if (fd == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }

    if (int ret = fcntl(fd, F_GLOBAL_NOCACHE, 1); ret == -1) {
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
    const char* fileName = removeFileCommand.fileName;
    if (int ret = unlink(fileName); ret == -1) {
        errCode = errnoToErrCode(errno);
        return false;
    }
    return true;
}

bool processAppendBlockCommand(const AppendBlockCommand& appendBlockCommand) {
    auto& [fileName, bytes, from] = appendBlockCommand;

    int oflag = O_WRONLY | O_APPEND;
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
    auto& [fileName, blockId, bytes, from] = writeBlockCommand;

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
    auto& [fileName, blockId, bytes, to] = readBlockCommannd;

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
    DiskIOThreadState actualCurrentState = _state;
    SPDLOG_INFO("Updating disk thread state from {} to {}", expectCurrentState, newState);
    DEBUG_ASSERT(expectCurrentState == actualCurrentState,
                 "Current expected state {}, actual state {}",
                 expectCurrentState,
                 actualCurrentState);
    _state = newState;
}

std::future<DiskCommandResult> DiskSpaceManager::submit(const DiskCommand& diskCommand) {
    std::promise<DiskCommandResult> reportPromise;
    auto reportFuture = reportPromise.get_future();
    submit(DiskCommandsGroup({diskCommand}, std::move(reportPromise)));
    return reportFuture;
}

std::future<DiskCommandResult> DiskSpaceManager::submit(const std::vector<DiskCommand>& diskCommands) {
    std::promise<DiskCommandResult> reportPromise;
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
        if (_state == DiskIOThreadState::Stopping) {
            SPDLOG_WARN("Stop command detected, will stop after finishing remaining I/O commands");
        }

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
        for (auto& diskCommand : diskCommandsGroup->diskCommands) {
            SPDLOG_INFO("Process disk command: {}", diskCommand);
            bool success = processDiskCommand(diskCommand);
            SPDLOG_INFO("Disk command result: Success = {} {}", success, success ? "" : strErrCode(errCode));

            if (!success) {
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

std::ostream& operator<<(std::ostream& os, const DiskSpaceManager::DiskIOThreadState& diskIoThreadState) {
    switch (diskIoThreadState) {
        case DiskSpaceManager::DiskIOThreadState::Open:
            os << "Open";
            break;
        case DiskSpaceManager::DiskIOThreadState::Running:
            os << "Running";
            break;
        case DiskSpaceManager::DiskIOThreadState::Stopping:
            os << "Stopping";
            break;
        case DiskSpaceManager::DiskIOThreadState::Stopped:
            os << "Stopped";
            break;
        default:
            RELEASE_ABORT("Illegal disk io thread state");
    }
    return os;
}

std::future<DiskCommandResult> createFile(BorrowedPointer<DiskSpaceManager> diskSpaceManager, const char* fileName) {
    return diskSpaceManager->submit(CreateFileCommand(fileName));
}

std::future<DiskCommandResult> removeFile(BorrowedPointer<DiskSpaceManager> diskSpaceManager, const char* fileName) {
    return diskSpaceManager->submit(RemoveFileCommand(fileName));
}

std::future<DiskCommandResult> appendBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                                           const char* fileName,
                                           BlockBytes bytes,
                                           const void* from) {
    return diskSpaceManager->submit(AppendBlockCommand(fileName, bytes, from));
}

std::future<DiskCommandResult> writeBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                                          const char* fileName,
                                          BlockId blockId,
                                          BlockBytes bytes,
                                          const void* from) {
    return diskSpaceManager->submit(WriteBlockCommand(fileName, blockId, bytes, from));
}

std::future<DiskCommandResult> readBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                                         const char* fileName,
                                         BlockId blockId,
                                         BlockBytes bytes,
                                         void* to) {
    return diskSpaceManager->submit(ReadBlockCommannd(fileName, blockId, bytes, to));
}

} // namespace diskspace