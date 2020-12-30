#include "diskspace/DiskCommands.hpp"

#include <type_traits>
#include <variant>

namespace diskspace {
std::ostream& operator<<(std::ostream& os, const DiskCommand& diskCommand) {
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

std::ostream& operator<<(std::ostream& os, const CreateFileCommand& createFileCommand) {
    os << "CreateFileCommand {fileName = " << createFileCommand.fileName << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const RemoveFileCommand& removeFileCommand) {
    os << "RemoveFileCommand {fileName = " << removeFileCommand.fileName << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const AppendBlockCommand& appendBlockCommand) {
    os << "AppendBlockCommand {fileName = " << appendBlockCommand.fileName << ", bytes = " << appendBlockCommand.bytes
       << ", from = " << uintptr_t(appendBlockCommand.from) << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const WriteBlockCommand& writeBlockCommand) {
    os << "WriteBlockCommand {fileName = " << writeBlockCommand.fileName << ", blockId = " << writeBlockCommand.blockId
       << ", bytes = " << writeBlockCommand.bytes << ", from " << uintptr_t(writeBlockCommand.from) << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const ReadBlockCommannd& readBlockCommannd) {
    os << "ReadBlockCommand {fileName = " << readBlockCommannd.fileName << ", blockId = " << readBlockCommannd.blockId
       << ", bytes = " << readBlockCommannd.bytes << ", to = " << uintptr_t(readBlockCommannd.to) << "}";
    return os;
}

} // namespace diskspace