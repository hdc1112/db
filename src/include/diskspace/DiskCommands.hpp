#pragma once

#include "TypeAlias.hpp"

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

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
    AppendBlockCommand(const char* fileName, BlockBytes bytes, const uint8_t* const from)
        : fileName(fileName), bytes(bytes), from(from) {}
    const char* fileName;
    BlockBytes bytes;
    const uint8_t* const from;
};

struct WriteBlockCommand {
    WriteBlockCommand(const char* fileName, BlockId blockId, BlockBytes bytes, const uint8_t* const from)
        : fileName(fileName), blockId(blockId), bytes(bytes), from(from) {}
    const char* fileName;
    BlockId blockId;
    BlockBytes bytes;
    const uint8_t* const from;
};

struct ReadBlockCommannd {
    ReadBlockCommannd(const char* fileName, BlockId blockId, BlockBytes bytes, uint8_t* const to)
        : fileName(fileName), blockId(blockId), bytes(bytes), to(to) {}
    const char* fileName;
    BlockId blockId;
    BlockBytes bytes;
    uint8_t* const to;
};

using DiskCommand =
    std::variant<CreateFileCommand, RemoveFileCommand, AppendBlockCommand, WriteBlockCommand, ReadBlockCommannd>;

std::ostream& operator<<(std::ostream& os, const CreateFileCommand& createFileCommand);
std::ostream& operator<<(std::ostream& os, const RemoveFileCommand& removeFileCommand);
std::ostream& operator<<(std::ostream& os, const AppendBlockCommand& appendBlockCommand);
std::ostream& operator<<(std::ostream& os, const WriteBlockCommand& writeBlockCommand);
std::ostream& operator<<(std::ostream& os, const ReadBlockCommannd& readBlockCommannd);

std::ostream& operator<<(std::ostream& argument, const DiskCommand& diskCommand);
} // namespace diskspace