#include <future>

#include "FutureTestUtility.hpp"

#include "gtest/gtest.h"

namespace utils {
namespace {
TEST(FutureFunctionTest, CannotOperateOnFutureAfterCallingGet) {
    std::promise<void> promise;
    promise.set_value();
    std::future<void> future = promise.get_future();
    WAIT_FOR(future);
    future.get();
    ASSERT_EXIT(future.get(), ::testing::KilledBySignal(SIGSEGV), ".*");
    ASSERT_EXIT(future.wait(), ::testing::KilledBySignal(SIGSEGV), ".*");
}
} // namespace
} // namespace utils