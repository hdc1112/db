#pragma once

#include <type_traits>

namespace utils {
template<typename Callable>
class ScopeGuard {
public:
    explicit ScopeGuard(Callable callable) : _callable(callable) {}
    ~ScopeGuard() {
        _callable();
    }

private:
    Callable _callable;
};
} // namespace utils