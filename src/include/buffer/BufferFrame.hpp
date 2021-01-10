#pragma once

#include "buffer/BufferPoolTypes.hpp"
#include "buffer/EvictPolicy.hpp"
#include "buffer/MemoryRegion.hpp"
#include "utility/BorrowedPointer.hpp"

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
class BufferFrame {
public:
    BufferFrame() = default;
    BufferFrame(FrameId frameId,
                FrameBytes frameBytes,
                diskspace::BlockId blockId,
                utils::BorrowedPointer<MemoryRegion> memoryRegion)
        : _frameId(frameId),
          _frameBytes(frameBytes),
          _blockId(blockId),
          _dirty(false),
          _pinCount(1),
          _memoryRegion(std::move(memoryRegion)) {}

    // -- Getters

    [[nodiscard]] bool pinned() const {
        return _pinCount > 0;
    }

    [[nodiscard]] bool isDirty() const {
        return _dirty;
    }

    [[nodiscard]] bool valid() const {
        return _memoryRegion != nullptr;
    }

    [[nodiscard]] diskspace::BlockId getBlockid() const {
        return _blockId;
    }

    [[nodiscard]] utils::BorrowedPointer<MemoryRegion> getMemoryRegion() const {
        return _memoryRegion;
    }

    // - Setters

    void setDirty(bool dirty) {
        _dirty = dirty;
    }

private:
    FrameId _frameId;
    FrameBytes _frameBytes;

    // The blockId is only used to flush this buffer frame. It is invisible to buffer's customer.
    diskspace::BlockId _blockId;

    bool _dirty;

    // Buffer frame is created only when loading a disk block, so count is init to 1.
    PinCount _pinCount;
    utils::BorrowedPointer<MemoryRegion> _memoryRegion;
};
} // namespace buffer