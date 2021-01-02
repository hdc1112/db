#pragma once

#include "Assert.hpp"
#include "ErrCode.hpp"

#include "FutureTestUtility.hpp"
#include "diskspace/DiskSpaceManager.hpp"

#include <utility>

namespace diskspace {
// Blocking calls wrapper, capitalize the first letter
inline DiskCommandResult CreateFile(const BorrowedPointer<DiskSpaceManager>& diskSpaceManager, const char* fileName) {
    auto future = createFile(diskSpaceManager, fileName);
    WAIT_FOR(future);
    return future.get();
}

inline DiskCommandResult RemoveFile(const BorrowedPointer<DiskSpaceManager>& diskSpaceManager, const char* fileName) {
    auto future = removeFile(diskSpaceManager, fileName);
    WAIT_FOR(future);
    return future.get();
}

inline DiskCommandResult AppendBlock(const BorrowedPointer<DiskSpaceManager>& diskSpaceManager,
                                     const char* fileName,
                                     BlockBytes bytes,
                                     const uint8_t* from) {
    auto future = appendBlock(diskSpaceManager, fileName, bytes, from);
    WAIT_FOR(future);
    return future.get();
}

inline DiskCommandResult WriteBlock(const BorrowedPointer<DiskSpaceManager>& diskSpaceManager,
                                    const char* fileName,
                                    BlockId blockId,
                                    BlockBytes bytes,
                                    const uint8_t* from) {
    auto future = writeBlock(diskSpaceManager, fileName, blockId, bytes, from);
    WAIT_FOR(future);
    return future.get();
}

inline DiskCommandResult ReadBlock(const BorrowedPointer<DiskSpaceManager>& diskSpaceManager,
                                   const char* fileName,
                                   BlockId blockId,
                                   BlockBytes bytes,
                                   uint8_t* to) {
    auto future = readBlock(diskSpaceManager, fileName, blockId, bytes, to);
    WAIT_FOR(future);
    return future.get();
}

// Blocking call and assert success

inline void AssertCreateFile(const BorrowedPointer<DiskSpaceManager>& diskSpaceManager, const char* fileName) {
    auto diskCommandResult = CreateFile(diskSpaceManager, fileName);
    DEBUG_ASSERT(diskCommandResult.success, "{}", strErrCode(diskCommandResult.errCode));
}

inline void AssertRemoveFile(const BorrowedPointer<DiskSpaceManager>& diskSpaceManager, const char* fileName) {
    auto diskCommandResult = RemoveFile(diskSpaceManager, fileName);
    DEBUG_ASSERT(diskCommandResult.success, "{}", strErrCode(diskCommandResult.errCode));
}

inline void AssertAppendBlock(const BorrowedPointer<DiskSpaceManager>& diskSpaceManager,
                              const char* fileName,
                              BlockBytes bytes,
                              const uint8_t* from) {
    auto diskCommandResult = AppendBlock(diskSpaceManager, fileName, bytes, from);
    DEBUG_ASSERT(diskCommandResult.success, "{}", strErrCode(diskCommandResult.errCode));
}

inline void AssertWriteBlock(const BorrowedPointer<DiskSpaceManager>& diskSpaceManager,
                             const char* fileName,
                             BlockId blockId,
                             BlockBytes bytes,
                             const uint8_t* from) {
    auto diskCommandResult = WriteBlock(diskSpaceManager, fileName, blockId, bytes, from);
    DEBUG_ASSERT(diskCommandResult.success, "{}", strErrCode(diskCommandResult.errCode));
}

inline void AssertReadBlock(const BorrowedPointer<DiskSpaceManager>& diskSpaceManager,
                            const char* fileName,
                            BlockId blockId,
                            BlockBytes bytes,
                            uint8_t* to) {
    auto diskCommandResult = ReadBlock(diskSpaceManager, fileName, blockId, bytes, to);
    DEBUG_ASSERT(diskCommandResult.success, "{}", strErrCode(diskCommandResult.errCode));
}

} // namespace diskspace