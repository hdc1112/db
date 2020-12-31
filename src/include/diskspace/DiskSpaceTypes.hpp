#pragma once

#include "ErrCode.hpp"

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <utility>

namespace diskspace {
using BlockNum = uint32_t;
using BlockId = uint32_t;
using BlockBytes = uint32_t;

struct DiskCommandResult {
    DiskCommandResult() : success(true), errCode(ERR_NO_ERROR) {}
    DiskCommandResult(bool success, ErrCode errCode) : success(success), errCode(errCode) {}
    bool success;
    ErrCode errCode;
};

inline std::ostream& operator<<(std::ostream& os, const diskspace::DiskCommandResult& diskCommandResult) {
    os << "DiskCommandResult {Success = " << diskCommandResult.success;
    if (diskCommandResult.success) {
        os << "}";
    } else {
        os << ", Error Message = " << strErrCode(diskCommandResult.errCode) << "}";
    }
    return os;
}

constexpr BlockBytes operator"" _KB(unsigned long long num) {
    return num * 1024;
}

constexpr BlockBytes operator"" _MB(unsigned long long num) {
    return num * 1024 * 1024;
}

constexpr BlockBytes operator"" _GB(unsigned long long num) {
    return num * 1024 * 1024 * 1024;
}
} // namespace diskspace
