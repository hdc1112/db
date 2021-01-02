#include "utility/BorrowedPointer.hpp"

#include "gtest/gtest.h"

#include <memory>

namespace utils {
namespace {
class Dummy : public BorrowableObject<Dummy> {
public:
    Dummy() = default;
    int getValue() {
        return 1;
    }
};

TEST(BorrowedPointerTest, sanityTest) {
    std::unique_ptr<Dummy> dummy = std::make_unique<Dummy>();
    {
        EXPECT_EQ(0, dummy->borrowCount());
        BorrowedPointer<Dummy> b1 = dummy->borrow();
        EXPECT_EQ(1, dummy->borrowCount());
        BorrowedPointer<Dummy> b2 = dummy->borrow();
        EXPECT_EQ(2, dummy->borrowCount());
        BorrowedPointer<Dummy> b3(b1);
        EXPECT_EQ(3, dummy->borrowCount());
        EXPECT_EQ(1, b3->getValue());
        b3 = std::move(b2);
        EXPECT_EQ(2, dummy->borrowCount());
        EXPECT_EQ(nullptr, b2.get());
        BorrowedPointer<Dummy> b4(std::move(b3));
        EXPECT_EQ(2, dummy->borrowCount());
    }
    EXPECT_EQ(0, dummy->borrowCount());
}
} // namespace
} // namespace utils