#pragma once

#include "buffer/BufferFrame.hpp"
#include "buffer/BufferPoolTypes.hpp"

namespace buffer {
class BufferFrameClock : public BufferFrame {
public:
    BufferFrameClock() = default;
    BufferFrameClock(FrameId frameId,
                     FrameBytes frameBytes,
                     diskspace::BlockId blockId,
                     utils::BorrowedPointer<MemoryRegion> memoryRegion)
        : BufferFrame(frameId, frameBytes, blockId, std::move(memoryRegion)), _refBit(true) {}

    void setRefBit() {
        _refBit = true;
    }
    void clearRefBit() {
        _refBit = false;
    }
    [[nodiscard]] ClockRefBit getRefBit() const {
        return _refBit;
    }

private:
    ClockRefBit _refBit;
};
} // namespace buffer