#pragma once

#include "BuildFlagsGenerated.hpp"

#include "spdlog/spdlog.h"

inline void printBuildFlags() {
    spdlog::info("kDebugAssertCrashIfFail: {}", kDebugAssertCrashIfFail);
    spdlog::info("kReleaseAssertCrashIfFail: {}", kReleaseAssertCrashIfFail);
}