#pragma once

#include "ErrCode.hpp"
#include "TypeAlias.hpp"
#include "diskspace/DiskCommands.hpp"
#include "utility/BlockingQueue.hpp"

#include "spdlog/spdlog.h"

#include <chrono>
#include <future>
#include <string>

using namespace std::chrono_literals;

namespace diskspace {

class DiskSpaceManager {
public:
    void start();
    std::future<void> stop();

    std::future<DiskCommandResult> submit(const DiskCommand& diskCommand);
    std::future<DiskCommandResult> submit(const std::vector<DiskCommand>& diskCommands);

private:
    struct DiskCommandsGroup {
        DiskCommandsGroup(std::vector<DiskCommand> diskCommands, std::promise<DiskCommandResult>&& reportPromise)
            : diskCommands(std::move(diskCommands)), reportPromise(std::move(reportPromise)) {}
        std::vector<DiskCommand> diskCommands;
        std::promise<DiskCommandResult> reportPromise;
    };

    enum class DiskIOThreadState : short { Open, Running, Stopping, Stopped };

    void submit(DiskCommandsGroup diskCommandsGroup);

    void run();

    void transferState(DiskIOThreadState expectCurrentState, DiskIOThreadState newState);

    volatile DiskIOThreadState _state = DiskIOThreadState::Open;
    utils::BlockingQueue<DiskCommandsGroup> _q;
    std::thread _thread;
    std::promise<void> _stopPromise;

    constexpr static std::chrono::milliseconds dequeueTimeout = 100ms;
};

std::future<DiskCommandResult> createFile(DiskSpaceManager* diskSpaceManager, const char* fileName);
std::future<DiskCommandResult> removeFile(DiskSpaceManager* diskSpaceManager, const char* fileName);
std::future<DiskCommandResult> appendBlock(DiskSpaceManager* diskSpaceManager,
                                           const char* fileName,
                                           BlockBytes bytes,
                                           const uint8_t* from);
std::future<DiskCommandResult> writeBlock(
    DiskSpaceManager* diskSpaceManager, const char* fileName, BlockId blockId, BlockBytes bytes, const uint8_t* from);
std::future<DiskCommandResult> readBlock(
    DiskSpaceManager* diskSpaceManager, const char* fileName, BlockId blockId, BlockBytes bytes, uint8_t* to);
} // namespace diskspace