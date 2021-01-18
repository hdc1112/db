#pragma once

#include "buffer/BufferFrame.hpp"
#include "buffer/BufferPoolTypes.hpp"

namespace buffer {
class BufferFrameClock : public BufferFrame<EvictPolicy::CLOCK> {
public:
    BufferFrameClock() = default;
    BufferFrameClock(FrameId frameId, diskspace::BlockId blockId, std::vector<uint8_t> memory)
        : BufferFrame(frameId, blockId, std::move(memory)), _refBit(true) {}

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
    ClockRefBit _refBit{};
};
} // namespace buffer