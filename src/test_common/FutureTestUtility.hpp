#pragma once

#include "Assert.hpp"

#include "TestParameters.hpp"

#include <future>
#include <type_traits>

// Do not use this macro like: WAIT_FOR(readBlock(fileName, ..)) since it will replace the readBlock function twice
// The safe way to use this macro is: auto future = readBlock(...); WAIT_FOR(future)
#define WAIT_FOR(future)                                                                                               \
    do {                                                                                                               \
        if (future.valid()) {                                                                                          \
            if (future.wait_for(g_waitForMillis) != std::future_status::ready) {                                       \
                DEBUG_ABORT("future.wait_for() timeout after millis {}", g_waitForMillis.count());                     \
            }                                                                                                          \
        } else {                                                                                                       \
            DEBUG_ABORT("future is not valid, wait() operation cannot proceed");                                       \
        }                                                                                                              \
    } while (false)
