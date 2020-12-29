#include "utility/BlockingQueue.hpp"

#include "gtest/gtest.h"

#include <thread>

namespace utils {
namespace {

TEST(BlockingQueueTest, sanityTest) {
    BlockingQueue<int> q;
    int elementNum = 100;
    std::thread producer([&q, elementNum]() {
        for (int i = 0; i < elementNum; ++i) {
            q.enqueue(i);
        }
    });
    std::thread consumer([&q, elementNum]() {
        for (int i = 0; i < elementNum; ++i) {
            EXPECT_EQ(i, q.dequeue());
        }
    });

    producer.join();
    consumer.join();
}
} // namespace
} // namespace utils