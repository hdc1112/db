#pragma once

#include "diskspace/DiskSpaceTypes.hpp"

namespace buffer {
using FrameNum = std::size_t;
using FrameId = std::size_t;
using FrameBytes = diskspace::BlockBytes;
using ClockRefBit = bool;
using PinCount = std::size_t;
} // namespace buffer