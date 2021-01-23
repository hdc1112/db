#include "Log.hpp"

#include "async++.h"
#include "gtest/gtest.h"

namespace {
TEST(AsyncplusplusTest, OneDefaultSchedTask) {
    auto myTask = async::spawn([] { return 42; }).then([] { return 43; });
    SPDLOG_DEBUG("myTask.get() = {}", myTask.get());
}
} // namespace