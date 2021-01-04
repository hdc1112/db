#pragma once

#include <future>

namespace utils {
template<typename T>
inline std::future<T> completedFuture() {
    std::promise<T> promise;
    promise.set_value(T());
    return promise.get_future();
}

} // namespace utils