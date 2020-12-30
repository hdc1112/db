#pragma once

#include "ErrCode.hpp"
#include "TypeAlias.hpp"
#include "diskspace/DiskCommands.hpp"
#include "diskspace/DiskSpaceConstants.hpp"
#include "utility/BlockingQueue.hpp"

#include "spdlog/spdlog.h"

#include <chrono>
#include <future>
#include <string>

using namespace std::chrono_literals;

namespace diskspace {

struct DiskCommandsGroup {
    DiskCommandsGroup(std::vector<DiskCommand> diskCommands, std::promise<DiskCommandReport>&& reportPromise)
        : diskCommands(std::move(diskCommands)), reportPromise(std::move(reportPromise)) {}
    std::vector<DiskCommand> diskCommands;
    std::promise<DiskCommandReport> reportPromise;
};

enum class DiskIOThreadState : short { Open, Running, Stopping, Stopped };

constexpr std::chrono::milliseconds dequeueTimeout = 100ms;

class DiskSpaceManager {
public:
    void start();
    std::future<void> stop();

    std::future<DiskCommandReport> submit(const DiskCommand& diskCommand);
    std::future<DiskCommandReport> submit(const std::vector<DiskCommand>& diskCommands);

private:
    void submit(DiskCommandsGroup diskCommandsGroup);

    void run();

    void transferState(DiskIOThreadState expectCurrentState, DiskIOThreadState newState);

    volatile DiskIOThreadState _state = DiskIOThreadState::Open;
    utils::BlockingQueue<DiskCommandsGroup> _q;
    std::thread _thread;
    std::promise<void> _stopPromise;
};
} // namespace diskspace