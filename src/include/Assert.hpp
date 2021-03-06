#pragma once

#include "BuildFlagsGenerated.hpp"

#include "Log.hpp"

#include <cassert>

#define DEBUG_ASSERT(flag, fmt, ...)                                                                                   \
    do {                                                                                                               \
        if (!(flag)) {                                                                                                 \
            SPDLOG_ERROR(fmt, ##__VA_ARGS__);                                                                          \
            if (kDebugAssertCrashIfFail) {                                                                             \
                assert(false);                                                                                         \
                std::terminate();                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (false)

#define DEBUG_ABORT(fmt, ...) DEBUG_ASSERT(false, fmt, ##__VA_ARGS__)

#define RELEASE_ASSERT(flag, fmt, ...)                                                                                 \
    do {                                                                                                               \
        if (!(flag)) {                                                                                                 \
            SPDLOG_CRITICAL(fmt, ##__VA_ARGS__);                                                                       \
            if (kReleaseAssertCrashIfFail) {                                                                           \
                assert(false);                                                                                         \
                std::terminate();                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (false)

#define RELEASE_ABORT(fmt, ...) RELEASE_ASSERT(false, fmt, ##__VA_ARGS__)
