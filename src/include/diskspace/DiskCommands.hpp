#pragma once

#include "diskspace/DiskSpaceTypes.hpp"

#include "Log.hpp"

#include <variant>

namespace diskspace {
struct CreateFileCommand {
    explicit CreateFileCommand(const char* fileName) : fileName(fileName) {}
    const char* fileName;
};

struct RemoveFileCommand {
    explicit RemoveFileCommand(const char* fileName) : fileName(fileName) {}
    const char* fileName;
};

struct AppendBlockCommand {
    AppendBlockCommand(const char* fileName, BlockBytes bytes, const void* from)
        : fileName(fileName), bytes(bytes), from(from) {}
    const char* fileName;
    BlockBytes bytes;
    const void* from;
};

struct WriteBlockCommand {
    WriteBlockCommand(const char* fileName, BlockId blockId, BlockBytes bytes, const void* from)
        : fileName(fileName), blockId(blockId), bytes(bytes), from(from) {}
    const char* fileName;
    BlockId blockId;
    BlockBytes bytes;
    const void* from;
};

struct ReadBlockCommannd {
    ReadBlockCommannd(const char* fileName, BlockId blockId, BlockBytes bytes, void* to)
        : fileName(fileName), blockId(blockId), bytes(bytes), to(to) {}
    const char* fileName;
    BlockId blockId;
    BlockBytes bytes;
    void* to;
};

using DiskCommand =
    std::variant<CreateFileCommand, RemoveFileCommand, AppendBlockCommand, WriteBlockCommand, ReadBlockCommannd>;

inline std::ostream& operator<<(std::ostream& os, const CreateFileCommand& createFileCommand) {
    os << "CreateFileCommand {fileName = " << createFileCommand.fileName << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const RemoveFileCommand& removeFileCommand) {
    os << "RemoveFileCommand {fileName = " << removeFileCommand.fileName << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const AppendBlockCommand& appendBlockCommand) {
    os << "AppendBlockCommand {fileName = " << appendBlockCommand.fileName << ", bytes = " << appendBlockCommand.bytes
       << ", from = " << fmt::ptr(appendBlockCommand.from) << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const WriteBlockCommand& writeBlockCommand) {
    os << "WriteBlockCommand {fileName = " << writeBlockCommand.fileName << ", blockId = " << writeBlockCommand.blockId
       << ", bytes = " << writeBlockCommand.bytes << ", from " << fmt::ptr(writeBlockCommand.from) << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ReadBlockCommannd& readBlockCommannd) {
    os << "ReadBlockCommand {fileName = " << readBlockCommannd.fileName << ", blockId = " << readBlockCommannd.blockId
       << ", bytes = " << readBlockCommannd.bytes << ", to = " << fmt::ptr(readBlockCommannd.to) << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const DiskCommand& diskCommand) {
    return std::visit(
        [&os](auto&& argument) -> std::ostream& {
            using T = std::decay_t<decltype(argument)>;
            if constexpr (std::is_same_v<T, CreateFileCommand>) {
                os << CreateFileCommand(argument);
            } else if constexpr (std::is_same_v<T, RemoveFileCommand>) {
                os << RemoveFileCommand(argument);
            } else if constexpr (std::is_same_v<T, AppendBlockCommand>) {
                os << AppendBlockCommand(argument);
            } else if constexpr (std::is_same_v<T, WriteBlockCommand>) {
                os << WriteBlockCommand(argument);
            } else if constexpr (std::is_same_v<T, ReadBlockCommannd>) {
                os << ReadBlockCommannd(argument);
            } else {
                os << "Unknown disk command";
            }
            return os;
        },
        diskCommand);
}

} // namespace diskspace