#pragma once

#include "ErrCode.hpp"
#include "diskspace/DiskCommands.hpp"
#include "diskspace/DiskSpaceTypes.hpp"
#include "utility/BlockingQueue.hpp"
#include "utility/BorrowedPointer.hpp"

#include "Log.hpp"

#include <chrono>
#include <future>
#include <string>

using namespace std::chrono_literals;

namespace diskspace {

using utils::BorrowableObject;
using utils::BorrowedPointer;

class DiskSpaceManager : public BorrowableObject<DiskSpaceManager> {
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
    friend std::ostream& operator<<(std::ostream& os, const DiskIOThreadState& diskIoThreadState);

    void submit(DiskCommandsGroup diskCommandsGroup);

    void run();

    void transferState(DiskIOThreadState expectCurrentState, DiskIOThreadState newState);

    volatile DiskIOThreadState _state = DiskIOThreadState::Open;
    utils::BlockingQueue<DiskCommandsGroup> _q;
    std::thread _thread;
    std::promise<void> _stopPromise;

    constexpr static std::chrono::milliseconds dequeueTimeout = 100ms;
};

std::future<DiskCommandResult> createFile(BorrowedPointer<DiskSpaceManager> diskSpaceManager, const char* fileName);
std::future<DiskCommandResult> removeFile(BorrowedPointer<DiskSpaceManager> diskSpaceManager, const char* fileName);
std::future<DiskCommandResult> appendBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                                           const char* fileName,
                                           BlockBytes bytes,
                                           const void* buffer);
std::future<DiskCommandResult> writeBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                                          const char* fileName,
                                          BlockId blockId,
                                          BlockBytes bytes,
                                          const void* from);
std::future<DiskCommandResult> readBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                                         const char* fileName,
                                         BlockId blockId,
                                         BlockBytes bytes,
                                         void* to);
} // namespace diskspace