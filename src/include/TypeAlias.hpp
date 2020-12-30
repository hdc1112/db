#pragma once

#include <cstdint>
#include <utility>

using ErrCode = unsigned short;

namespace diskspace {
using BlockNum = uint32_t;
using BlockId = uint32_t;
using BlockBytes = uint32_t;
using DiskCommandResult = std::pair<bool, ErrCode>;
} // namespace diskspace