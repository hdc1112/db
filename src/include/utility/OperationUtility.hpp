#pragma once

namespace utils {
template<typename Callable>
void repeat(Callable callable, int n) {
    for (int i = 0; i < n; ++i) {
        callable();
    }
}
} // namespace utils