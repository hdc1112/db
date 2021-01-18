#include "buffer/clock/BufferPoolManagerClock.hpp"
#include "buffer/BufferParameters.hpp"
#include "buffer/clock/BufferFrameClock.hpp"
#include "diskspace/DiskSpaceTypes.hpp"
#include "utility/FutureUtility.hpp"

namespace buffer {
utils::BorrowedPointer<BufferFrame<EvictPolicy::CLOCK>> BufferPoolManagerClock::getBufferFrame(
    diskspace::BlockId blockId) {
    if (auto rawPageIter = _sysPages.find(blockId); rawPageIter != _sysPages.end()) {
        return rawPageIter->second->borrow();
    } else if (auto numOfFrames = totalFrames(); numOfFrames < _maxFrameNum) {
        FrameId newFrameId = numOfFrames;
        std::vector<uint8_t> memory(_frameBytes);
        auto future = readBlock(_diskSpaceManager, _diskFileName.data(), blockId, _frameBytes, memory.data());
        if (auto status = future.wait_for(DISK_IO_WAIT_MILLIS); status == std::future_status::ready) {
            auto newBufferFrameClock = std::make_unique<BufferFrameClock>(newFrameId, blockId, memory);
            _sysPages.emplace(blockId, std::move(newBufferFrameClock));
            return _sysPages.at(blockId)->borrow();
        } else {
            errCode = ERR_IO_R_TIMEOUT;
            return _nullBufferFrame->borrow();
        }
    } else {
        DEBUG_ABORT("Evict not support yet");
        return _nullBufferFrame->borrow();
    }
}

void BufferPoolManagerClock::_flush() {
    SPDLOG_INFO("Begin to flush dirty pages to disk");
    std::vector<diskspace::DiskCommand> diskCommands;
    for (auto& [_, bufferFrameClock] : _sysPages) {
        // TODO: check pinned or not
        if (bufferFrameClock->isDirty()) {
            auto blockId = bufferFrameClock->getBlockid();
            void* from = bufferFrameClock->address();
            diskspace::WriteBlockCommand writeBlockCommand{_diskFileName.data(), blockId, _frameBytes, from};
            diskCommands.emplace_back(writeBlockCommand);
        }
    }
    auto diskCommandResult = _diskSpaceManager->submit(diskCommands);
    auto commandsWaitTime = diskCommands.size() * DISK_IO_WAIT_MILLIS;
    if (auto status = diskCommandResult.wait_for(commandsWaitTime); status == std::future_status::ready) {
        if (auto [success, errCode] = diskCommandResult.get(); success) {
            // TODO reset dirty bit
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
    _flush();
}
} // namespace buffer