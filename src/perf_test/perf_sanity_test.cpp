#include "utility/StopWatch.hpp"

#include "Log.hpp"
#include "gtest/gtest.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

namespace {
TEST(PerfTestSanityTest, stopWatch) {
    utils::StopWatch stopWatch;
    stopWatch.start();
    std::this_thread::sleep_for(123ms);
    stopWatch.stop();
    SPDLOG_INFO("Elapsed millis {}", stopWatch.elapsedMs().count());
}
} // namespace