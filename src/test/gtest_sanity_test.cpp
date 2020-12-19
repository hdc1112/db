#include "gtest/gtest.h"

namespace {
TEST(GTestSanityTest, TrivialTest) {
    ASSERT_EQ(1, 1);
}
}