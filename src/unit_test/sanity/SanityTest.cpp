#include "Log.hpp"

#include "rxcpp/rx.hpp"
#include "gtest/gtest.h"

namespace {
TEST(SanityTest, GTest) {
    ASSERT_EQ(1, 1);
}

TEST(SanityTest, RxCppTest) {
    rxcpp::observable<>::range(1, 5).subscribe([](int e) { SPDLOG_DEBUG("OnNext {}", e); },
                                               []() { SPDLOG_DEBUG("OnCompleted"); });
}
} // namespace
