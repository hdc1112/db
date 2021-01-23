#pragma once

#include "ErrCode.hpp"
#include "utility/BorrowedPointer.hpp"

#include "Log.hpp"
#include "spdlog/fmt/ostr.h"

#include <cstdint>
#include <utility>

namespace diskspace {
using BlockNum = std::size_t;
using BlockId = std::size_t;
using BlockBytes = std::size_t;

struct DiskCommandResult {
    DiskCommandResult() : success(true), errCode(ERR_NO_ERROR) {}
    DiskCommandResult(bool success, ErrCode errCode) : success(success), errCode(errCode) {}
    bool success;
    ErrCode errCode;
};

inline std::ostream& operator<<(std::ostream& os, const diskspace::DiskCommandResult& diskCommandResult) {
    os << fmt::format("DiskCommandResult {Success = {}", diskCommandResult.success);
    if (!diskCommandResult.success) {
        os << fmt::format(", Error Message = {}", strErrCode(diskCommandResult.errCode));
    }
    os << "}";
    return os;
}
} // namespace diskspace
