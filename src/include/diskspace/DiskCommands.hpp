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
    const char* fileName;
};

struct AppendBlockCommand {
    const char* fileName;
    BlockBytes bytes;
    const uint8_t* from;
};

struct WriteBlockCommand {
    const char* fileName;
    BlockNum blockId;
    BlockBytes bytes;
    const uint8_t* const from;
};

struct ReadBlockCommannd {
    const char* fileName;
    BlockNum blockId;
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