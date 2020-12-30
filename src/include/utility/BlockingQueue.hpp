#pragma once

#include "utility/StopWatch.hpp"

#include <chrono>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>

namespace utils {
template<typename T>
class BlockingQueue {
public:
    template<typename... Args>
    void enqueue(Args&&... args) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.emplace(std::forward<Args>(args)...);
        _condition.notify_all();
    }

    T dequeue() {
        std::unique_lock<std::mutex> uniqueLock(_mutex);
        while (_queue.empty()) {
            _condition.wait(uniqueLock);
        }
        T element = std::move(_queue.front());
        _queue.pop();
        return element;
    }

    std::optional<T> tryDequeue(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> uniqueLock(_mutex);
        utils::StopWatch stopWatch;
        stopWatch.start();
        while (_queue.empty() && stopWatch.elapsedMs() < timeout) {
            _condition.wait_for(uniqueLock, timeout);
        }
        stopWatch.stop();

        if (_queue.empty()) {
            return std::nullopt;
        } else {
            auto ret = std::make_optional(std::move(_queue.front()));
            _queue.pop();
            return ret;
        }
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _condition;
};
} // namespace utils