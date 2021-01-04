#pragma once

#include "Assert.hpp"
#include "ErrCode.hpp"

#include "FutureTestUtility.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include <utility>

namespace diskspace {
// Blocking calls wrapper, capitalize the first letter
inline DiskCommandResult CreateFile(BorrowedPointer<DiskSpaceManager> diskSpaceManager, const char* fileName) {
    auto future = createFile(std::move(diskSpaceManager), fileName);
    WAIT_FOR(future);
    return future.get();
}

inline DiskCommandResult RemoveFile(BorrowedPointer<DiskSpaceManager> diskSpaceManager, const char* fileName) {
    auto future = removeFile(std::move(diskSpaceManager), fileName);
    WAIT_FOR(future);
    return future.get();
}

inline DiskCommandResult AppendBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                                     const char* fileName,
                                     BlockBytes bytes,
                                     const void* from) {
    auto future = appendBlock(std::move(diskSpaceManager), fileName, bytes, from);
    WAIT_FOR(future);
    return future.get();
}

inline DiskCommandResult WriteBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                                    const char* fileName,
                                    BlockId blockId,
                                    BlockBytes bytes,
                                    const void* from) {
    auto future = writeBlock(std::move(diskSpaceManager), fileName, blockId, bytes, from);
    WAIT_FOR(future);
    return future.get();
}

inline DiskCommandResult ReadBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                                   const char* fileName,
                                   BlockId blockId,
                                   BlockBytes bytes,
                                   void* to) {
    auto future = readBlock(std::move(diskSpaceManager), fileName, blockId, bytes, to);
    WAIT_FOR(future);
    return future.get();
}

// Blocking call and assert success

inline void AssertCreateFile(BorrowedPointer<DiskSpaceManager> diskSpaceManager, const char* fileName) {
    auto diskCommandResult = CreateFile(std::move(diskSpaceManager), fileName);
    DEBUG_ASSERT(diskCommandResult.success, "{}", strErrCode(diskCommandResult.errCode));
}

inline void AssertRemoveFile(BorrowedPointer<DiskSpaceManager> diskSpaceManager, const char* fileName) {
    auto diskCommandResult = RemoveFile(std::move(diskSpaceManager), fileName);
    DEBUG_ASSERT(diskCommandResult.success, "{}", strErrCode(diskCommandResult.errCode));
}

inline void AssertAppendBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                              const char* fileName,
                              BlockBytes bytes,
                              void* from) {
    auto diskCommandResult = AppendBlock(std::move(diskSpaceManager), fileName, bytes, from);
    DEBUG_ASSERT(diskCommandResult.success, "{}", strErrCode(diskCommandResult.errCode));
}

inline void AssertWriteBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                             const char* fileName,
                             BlockId blockId,
                             BlockBytes bytes,
                             void* from) {
    auto diskCommandResult = WriteBlock(std::move(diskSpaceManager), fileName, blockId, bytes, from);
    DEBUG_ASSERT(diskCommandResult.success, "{}", strErrCode(diskCommandResult.errCode));
}

inline void AssertReadBlock(BorrowedPointer<DiskSpaceManager> diskSpaceManager,
                            const char* fileName,
                            BlockId blockId,
                            BlockBytes bytes,
                            void* to) {
    auto diskCommandResult = ReadBlock(std::move(diskSpaceManager), fileName, blockId, bytes, to);
    DEBUG_ASSERT(diskCommandResult.success, "{}", strErrCode(diskCommandResult.errCode));
}

} // namespace diskspace