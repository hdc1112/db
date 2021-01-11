#pragma once

#include "Assert.hpp"
#include "SizeTypes.hpp"
#include "buffer/BufferFrame.hpp"
#include "buffer/EvictPolicy.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include <chrono>
#include <unordered_map>

namespace buffer {

/**
 * BufferPoolManager
 *
 * BufferPoolManager manages in-memory buffer frames. Its main job is to return a buffer frame when given a PageId.
 * It is usually not useful to get a raw buffer frame, so this class provides a function template to return a page.
 */
class BufferPoolManager {
public:
    BufferPoolManager(EvictPolicy evictPolicy,
                      std::string_view diskFileName,
                      FrameNum maxFrameNum,
                      FrameBytes frameBytes,
                      utils::BorrowedPointer<diskspace::DiskSpaceManager> diskSpaceManager)
        : _evictPolicy(evictPolicy),
          _diskFileName(diskFileName),
          _maxFrameNum(maxFrameNum),
          _frameBytes(frameBytes),
          _diskSpaceManager(std::move(diskSpaceManager)) {}

    [[nodiscard]] EvictPolicy getEvictPolicy() const {
        return _evictPolicy;
    }

    template<typename PageType>
    PageType getPage(diskspace::BlockId blockId) {
        auto bufferFrame = getBufferFrame(blockId);
        return PageType(bufferFrame);
    }

protected:
    virtual BufferFrame getBufferFrame(diskspace::BlockId blockId) = 0;
    virtual void flush() = 0;

    EvictPolicy _evictPolicy;
    std::string_view _diskFileName;
    FrameNum _maxFrameNum;
    FrameBytes _frameBytes;
    utils::BorrowedPointer<diskspace::DiskSpaceManager> _diskSpaceManager;
};

} // namespace buffer