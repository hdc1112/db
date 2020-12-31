#pragma once

#include "Assert.hpp"

#include "TestParameters.hpp"

#include <future>
#include <type_traits>

#define WAIT_FOR(future)                                                                                               \
    do {                                                                                                               \
        if (future.wait_for(g_waitForMillis) != std::future_status::ready) {                                           \
            DEBUG_ABORT("future.wait_for() timeout after millis {}", g_waitForMillis.count());                         \
        }                                                                                                              \
    } while (false)

template<typename T>
inline std::future<T> completedFuture() {
    std::promise<T> promise;
    promise.set_value(T());
    return promise.get_future();
}
