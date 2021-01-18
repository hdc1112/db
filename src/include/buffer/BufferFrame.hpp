#pragma once

#include "buffer/BufferPoolTypes.hpp"
#include "buffer/EvictPolicy.hpp"
#include "utility/BorrowedPointer.hpp"

#include <vector>

namespace buffer {

/**
 * BufferFrame
 *
 * Buffer frame is the smallest management unit of a buffer pool manager. It contains a region of memory that has the
 * same size with a disk block. It is always written and read at the same time. It is ok if the buffer frame size
 * exceeds the atomic disk operation size, because the atomicity is guaranteed by recovery module. Different evict
 * policy demands different metadata or data structures.
 *
 * Note that usually one buffer frame is associated with one PageId, but not all disk file blocks are associated with
 * PageIds, for example, the 0-th block.
 *
 * Note that the customer of Buffer layer does not need to know any eviction policy specific data fields, so Buffer
 * pool manager always returns a BufferFrame instead of BufferFrame's subclasses.
 */
class BufferFrame : public utils::BorrowableObject<BufferFrame> {
public:
    BufferFrame() = default;
    BufferFrame(FrameId frameId,
                diskspace::BlockId blockId,
                std::vector<uint8_t> memory)
        : _frameId(frameId),
          _blockId(blockId),
          _dirty(false),
          _pinCount(1),
          _memory(std::move(memory)) {}

    // -- Getters

    [[nodiscard]] bool pinned() const {
        return _pinCount > 0;
    }

    [[nodiscard]] bool isDirty() const {
        return _dirty;
    }

    [[nodiscard]] bool valid() const {
        return !_memory.empty();
    }

    [[nodiscard]] diskspace::BlockId getBlockid() const {
        return _blockId;
    }

    void* address() {
        return _memory.data();
    }

    const void* address() const {
        return _memory.data();
    }

    FrameBytes size() const {
        return _memory.size();
    }

    // - Setters

    void setDirty(bool dirty) {
        _dirty = dirty;
    }

private:
    FrameId _frameId{};

    // The blockId is only used to flush this buffer frame. It is invisible to buffer's customer.
    diskspace::BlockId _blockId{};

    bool _dirty{};

    // Buffer frame is created only when loading a disk block, so count is init to 1.
    PinCount _pinCount{};
    std::vector<uint8_t> _memory;
};
} // namespace buffer