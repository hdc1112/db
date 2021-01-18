#pragma once

#include "buffer/BufferPoolManager.hpp"
#include "buffer/clock/BufferFrameClock.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include <memory>

namespace buffer {
class BufferPoolManagerClock : public BufferPoolManager<EvictPolicy::CLOCK> {
public:
    BufferPoolManagerClock(std::string_view diskFileName,
                           FrameNum maxFrameNum,
                           FrameBytes frameBytes,
                           utils::BorrowedPointer<diskspace::DiskSpaceManager> diskSpaceManager)
        : BufferPoolManager(diskFileName, maxFrameNum, frameBytes, std::move(diskSpaceManager)) {}

    ~BufferPoolManagerClock();

protected:
    utils::BorrowedPointer<BufferFrame> getBufferFrame(diskspace::BlockId blockId) override;
    void flush() override {
        _flush();
    }
    FrameNum totalFrames() override;

private:
    void _flush();

    std::unordered_map<diskspace::BlockId, std::unique_ptr<BufferFrameClock>> _sysPages{};
};
} // namespace buffer