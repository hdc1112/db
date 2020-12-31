#pragma once

#include "Assert.hpp"

#include "TestParameters.hpp"

#define WAIT_FOR(future)                                                                                               \
    do {                                                                                                               \
        if (future.wait_for(g_waitForMillis) != std::future_status::ready) {                                           \
            DEBUG_ABORT("future.wait_for() timeout after millis {}", g_waitForMillis.count());                         \
        }                                                                                                              \
    } while (false)

#define WAIT_DISK_CMD(runCommand)                                                                                      \
    do {                                                                                                               \
        auto future = (runCommand);                                                                                    \
        WAIT_FOR(future);                                                                                              \
    } while (false)

#define WAIT_DISK_CMD_AND_ASSERT(runCommand)                                                                           \
    do {                                                                                                               \
        auto future = (runCommand);                                                                                    \
        WAIT_FOR(future);                                                                                              \
        ASSERT_TRUE(future.get().success);                                                                             \
    } while (false)