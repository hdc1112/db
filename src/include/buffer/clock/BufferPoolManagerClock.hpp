#pragma once

#include "buffer/BufferPoolManager.hpp"
#include "buffer/clock/BufferFrameClock.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include <memory>

namespace buffer {
class BufferPoolManagerClock : public BufferPoolManager<EvictPolicy::CLOCK> {
public:
    using BufferPoolManager::BufferPoolManager;
    ~BufferPoolManagerClock() override;

protected:
    utils::BorrowedPointer<BufferFrame<EvictPolicy::CLOCK>> getBufferFrame(diskspace::BlockId blockId) override;
    void flush() override {
        _flush();
    }

private:
    void _flush();
};
} // namespace buffer