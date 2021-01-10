#include "buffer/clock/BufferPoolManagerClock.hpp"
#include "buffer/clock/BufferFrameClock.hpp"

namespace buffer {
BufferFrame BufferPoolManagerClock::getBufferFrame(diskspace::BlockId blockId) {
    if (auto rawPageIter = _rawPages.find(blockId); rawPageIter != _rawPages.end()) {
        return rawPageIter->second;
    } else if (_memoryRegions.size() < _maxFrameNum) {
        auto [newFrameId, newMemoryRegion] = allocateNewMemoryRegion();
        auto future =
            readBlock(_diskSpaceManager, _diskFileName.data(), blockId, _frameBytes, newMemoryRegion->address());
        if (auto status = future.wait_for(_ioWaitTime); status == std::future_status::ready) {
            BufferFrameClock newBufferFrame{newFrameId, _frameBytes, blockId, newMemoryRegion};
            _rawPages.emplace(blockId, std::move(newBufferFrame));
            return _rawPages.at(blockId);
        } else {
            errCode = ERR_IO_R_TIMEOUT;
            return BufferFrameClock();
        }
    } else {
        DEBUG_ABORT("Evict not support yet");
        return BufferFrameClock();
    }
}

std::pair<FrameId, utils::BorrowedPointer<MemoryRegion>> BufferPoolManagerClock::allocateNewMemoryRegion() {
    auto newFrameId = _memoryRegions.size();
    auto memoryRegion = std::make_unique<MemoryRegion>(_frameBytes);
    _memoryRegions.emplace_back(std::move(memoryRegion));
    return {newFrameId, _memoryRegions.back()->borrow()};
}

BufferPoolManagerClock::~BufferPoolManagerClock() {
    SPDLOG_INFO("Begin to flush dirty pages to disk");
    DEBUG_ASSERT(_rawPages.size() == _memoryRegions.size(), "Memory management issue");
    for (auto [_, bufferFrameClock] : _rawPages) {
        // TODO: check pinned
        if (bufferFrameClock.isDirty()) {
            auto blockId = bufferFrameClock.getBlockid();
            void* from = bufferFrameClock.getMemoryRegion()->address();
            auto future = writeBlock(_diskSpaceManager, _diskFileName.data(), blockId, _frameBytes, from);
            if (auto status = future.wait_for(_ioWaitTime); status == std::future_status::ready) {
                bufferFrameClock.setDirty(false);
            } else {
                errCode = ERR_IO_W_TIMEOUT;
                DEBUG_ABORT("Disk write time out");
            }
        }
    }
}
} // namespace buffer