#include <future>

#include "ErrCode.hpp"
#include "FutureTestUtility.hpp"
#include "utility/FutureUtility.hpp"

#include "gtest/gtest.h"

using namespace std::chrono_literals;

namespace utils {
namespace {
TEST(BasicFutureTest, CannotOperateOnFutureAfterCallingGet) {
    std::promise<void> promise;
    promise.set_value();
    std::future<void> future = promise.get_future();
    WAIT_FOR(future);
    future.get();
    ASSERT_EXIT(future.get(), ::testing::KilledBySignal(SIGSEGV), ".*");
    ASSERT_EXIT(future.wait(), ::testing::KilledBySignal(SIGSEGV), ".*");
}

TEST(BasicFutureTest, InitialFuture) {
    std::future<void> future;
    ASSERT_EXIT(future.get(), ::testing::KilledBySignal(SIGSEGV), ".*");
}

TEST(BasicFutureTest, InitialSharedFuture) {
    std::shared_future<void> future;
    ASSERT_EXIT(future.get(), ::testing::KilledBySignal(SIGSEGV), ".*");
}

TEST(BasicFutureTest, SharedFutureCopy) {
    std::promise<int> promise;
    promise.set_value(5);

    std::shared_future<int> future1 = promise.get_future();
    std::shared_future<int> future2 = future1;

    EXPECT_EQ(5, future1.get());
    EXPECT_EQ(5, future2.get());
}

TEST(BasicFutureTest, SharedFutureMove) {
    std::promise<int> promise;
    promise.set_value(5);

    std::shared_future<int> future1 = promise.get_future();
    std::shared_future<int> future2 = std::move(future1);

    EXPECT_FALSE(future1.valid());
    ASSERT_EXIT(future1.get(), ::testing::KilledBySignal(SIGSEGV), ".*");
    EXPECT_EQ(5, future2.get());
}

TEST(BasicFutureTest, CompositeFutureAllComplete) {
    utils::CompositeFuture<void> compositeFuture;
    compositeFuture.emplace(completedFuture<void>());
    compositeFuture.emplace(completedFuture<void>());
    compositeFuture.emplace(completedFuture<void>());
    compositeFuture.wait(1ms);
    EXPECT_EQ(errCode, ERR_NO_ERROR);
}

TEST(BasicFutureTest, CompositeFutureTimeout) {
    utils::CompositeFuture<int> compositeFuture;
    std::promise<int> promise1;
    compositeFuture.emplace(promise1.get_future());
    compositeFuture.emplace(completedFuture<int>());
    compositeFuture.wait(10ms);
    EXPECT_EQ(errCode, ERR_GENERAL_TIMEOUT);
}
} // namespace
} // namespace utils