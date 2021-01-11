#pragma once

#include <chrono>
#include <future>
#include <vector>

namespace utils {
template<typename T>
class CompositeFuture {
public:
    void emplace(std::future<T>&& f) {
        _futures.emplace_back(std::move(f));
    }

    std::vector<T> wait(std::chrono::milliseconds eachWaitMillis) {
        std::vector<T> results;
        for (std::future<T>& f : _futures) {
            if (auto status = f.wait_for(eachWaitMillis); status == std::future_status::ready) {
                results.emplace_back(f.get());
            } else {
                errCode = ERR_GENERAL_TIMEOUT;
                return {};
            }
        }
        return results;
    }

private:
    std::vector<std::future<T>> _futures{};
};

template<>
class CompositeFuture<void> {
public:
    void emplace(std::future<void>&& f) {
        _futures.emplace_back(std::move(f));
    }

    void wait(std::chrono::milliseconds eachWaitMillis) {
        errCode = ERR_NO_ERROR;
        for (std::future<void>& f : _futures) {
            if (auto status = f.wait_for(eachWaitMillis); status != std::future_status::ready) {
                errCode = ERR_GENERAL_TIMEOUT;
                return;
            }
        }
    }

private:
    std::vector<std::future<void>> _futures;
};

template<typename T>
inline std::future<T> completedFuture() {
    std::promise<T> promise;
    promise.set_value(T());
    return promise.get_future();
}

template<>
inline std::future<void> completedFuture() {
    std::promise<void> promise;
    promise.set_value();
    return promise.get_future();
}

} // namespace utils