#include "utility/BlockingQueue.hpp"

#include "gtest/gtest.h"

#include <thread>

namespace utils {
namespace {

TEST(BlockingQueueTest, sanityTest) {
    BlockingQueue<std::pair<int, long>> q;
    int elementNum = 100;
    std::thread producer([&q, elementNum]() {
        for (int i = 0; i < elementNum; ++i) {
            q.enqueue(i, long(i + 1));
        }
    });
    std::thread consumer([&q, elementNum]() {
        for (int i = 0; i < elementNum; ++i) {
            auto [e1, e2] = q.dequeue();
            EXPECT_EQ(i, e1);
            EXPECT_EQ(i + 1, e2);
        }
    });

    producer.join();
    consumer.join();
}
} // namespace
} // namespace utils