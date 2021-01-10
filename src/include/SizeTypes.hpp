#pragma once

constexpr std::size_t operator"" _KB(unsigned long long num) {
    return num * 1024;
}

constexpr std::size_t operator"" _MB(unsigned long long num) {
    return num * 1024 * 1024;
}

constexpr std::size_t operator"" _GB(unsigned long long num) {
    return num * 1024 * 1024 * 1024;
}
