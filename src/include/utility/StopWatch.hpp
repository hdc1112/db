#pragma once

#include <chrono>

namespace utils {
template<typename Clock>
class StopWatchGeneric {
    using duration = typename Clock::duration;
    using time_point = typename Clock::time_point;

public:
    void start() {
        if (!_running) {
            _startingTime = now();
            _running = true;
        }
    }

    void stop() {
        if (_running) {
            _elapsedTotal += fromStart();
            _running = false;
        }
    }

    void reset() {
        _elapsedTotal = {};
        _running = false;
    }

    void restart() {
        reset();
        start();
    }

    duration elapsed() const {
        auto elapsedDuration = _elapsedTotal;

        if (_running) {
            elapsedDuration += fromStart();
        }

        return elapsedDuration;
    }

    std::chrono::milliseconds elapsedMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed());
    }

    std::chrono::seconds elapsedSeconds() const {
        return std::chrono::duration_cast<std::chrono::seconds> > (elapsed());
    }

private:
    duration fromStart() const {
        return now() - _startingTime;
    }

    static time_point now() noexcept {
        return Clock::now();
    }

    duration _elapsedTotal{};
    time_point _startingTime{};
    bool _running = false;
};

using StopWatch = StopWatchGeneric<std::chrono::steady_clock>;
} // namespace utils