#pragma once

#include "buffer/BufferPoolManager.hpp"
#include "buffer/MemoryRegion.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include <memory>

namespace buffer {
class BufferPoolManagerClock : public BufferPoolManager {
public:
    BufferPoolManagerClock(std::string_view diskFileName,
                           FrameNum maxFrameNum,
                           FrameBytes frameBytes,
                           utils::BorrowedPointer<diskspace::DiskSpaceManager> diskSpaceManager)
        : BufferPoolManager(EvictPolicy::CLOCK, diskFileName, maxFrameNum, frameBytes),
          _memoryRegions({}),
          _diskSpaceManager(std::move(diskSpaceManager)) {}

    ~BufferPoolManagerClock();

protected:
    BufferFrame getBufferFrame(diskspace::BlockId blockId) override;

private:
    std::pair<FrameId, utils::BorrowedPointer<MemoryRegion>> allocateNewMemoryRegion();

    std::vector<std::unique_ptr<MemoryRegion>> _memoryRegions{};
    std::unordered_map<diskspace::BlockId, BufferFrameClock> _rawPages{};
    utils::BorrowedPointer<diskspace::DiskSpaceManager> _diskSpaceManager;
};
} // namespace buffer