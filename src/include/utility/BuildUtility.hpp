#pragma once

#include "BuildFlagsGenerated.hpp"

#include "spdlog/spdlog.h"

namespace utils {
inline void printBuildFlags() {
    SPDLOG_INFO("kDebugAssertCrashIfFail: {}", kDebugAssertCrashIfFail);
    SPDLOG_INFO("kReleaseAssertCrashIfFail: {}", kReleaseAssertCrashIfFail);
    SPDLOG_INFO("kBorrowedCounterCheck: {}", kBorrowedCounterCheck);
}
} // namespace utils