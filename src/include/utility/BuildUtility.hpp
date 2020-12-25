#pragma once

#include "BuildFlagsGenerated.hpp"

#include "spdlog/spdlog.h"

namespace utils {
inline void printBuildFlags() {
    spdlog::info("kDebugAssertCrashIfFail: {}", kDebugAssertCrashIfFail);
    spdlog::info("kReleaseAssertCrashIfFail: {}", kReleaseAssertCrashIfFail);
}
} // namespace utils