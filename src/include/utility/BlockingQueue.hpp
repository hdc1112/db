#pragma once

#include <mutex>
#include <queue>

namespace utils {
template<typename T>
class BlockingQueue {
public:
    template<typename E>
    void enqueue(E&& element) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.emplace(std::forward<E>(element));
        _condition.notify_all();
    }

    T dequeue() {
        std::unique_lock<std::mutex> uniqueLock(_mutex);
        while (_queue.empty()) {
            _condition.wait(uniqueLock);
        }
        T element = _queue.front();
        _queue.pop();
        return element;
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _condition;
};
} // namespace utils