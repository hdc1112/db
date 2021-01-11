#pragma once

#include <chrono>

using namespace std::chrono_literals;

namespace buffer {
constexpr std::chrono::milliseconds DISK_IO_WAIT_MILLIS = 100ms;
}