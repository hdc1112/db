#include "buffer/clock/BufferPoolManagerClock.hpp"
#include "buffer/BufferParameters.hpp"
#include "buffer/clock/BufferFrameClock.hpp"
#include "diskspace/DiskSpaceTypes.hpp"
#include "utility/FutureUtility.hpp"

namespace buffer {
BufferFrame BufferPoolManagerClock::getBufferFrame(diskspace::BlockId blockId) {
    if (auto rawPageIter = _rawPages.find(blockId); rawPageIter != _rawPages.end()) {
        return rawPageIter->second;
    } else if (_memoryRegions.size() < _maxFrameNum) {
        auto [newFrameId, newMemoryRegion] = allocateNewMemoryRegion();
        auto future =
            readBlock(_diskSpaceManager, _diskFileName.data(), blockId, _frameBytes, newMemoryRegion->address());
        if (auto status = future.wait_for(DISK_IO_WAIT_MILLIS); status == std::future_status::ready) {
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

void BufferPoolManagerClock::flush_() {
    SPDLOG_INFO("Begin to flush dirty pages to disk");
    DEBUG_ASSERT(_rawPages.size() == _memoryRegions.size(), "Memory management issue");
    std::vector<diskspace::DiskCommand> diskCommands;
    for (auto [_, bufferFrameClock] : _rawPages) {
        // TODO: check pinned or not
        if (bufferFrameClock.isDirty()) {
            auto blockId = bufferFrameClock.getBlockid();
            void* from = bufferFrameClock.getMemoryRegion()->address();
            diskspace::WriteBlockCommand writeBlockCommand{_diskFileName.data(), blockId, _frameBytes, from};
            diskCommands.emplace_back(writeBlockCommand);
        }
    }
    auto diskCommandResult = _diskSpaceManager->submit(diskCommands);
    auto commandsWaitTime = diskCommands.size() * DISK_IO_WAIT_MILLIS;
    if (auto status = diskCommandResult.wait_for(commandsWaitTime); status == std::future_status::ready) {
        if (auto [success, errCode] = diskCommandResult.get(); success) {
            SPDLOG_INFO("Successful flush");
        } else {
            SPDLOG_ERROR("Flush is not successful {}", strErrCode(errCode));
        }
    } else {
        errCode = ERR_IO_W_TIMEOUT;
        SPDLOG_ERROR("Flush is not successful {}", strErrCode(errCode));
    }
}

BufferPoolManagerClock::~BufferPoolManagerClock() {
    flush_();
}
} // namespace buffer