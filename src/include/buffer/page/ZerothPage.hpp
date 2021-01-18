#pragma once

#include "buffer/BufferFrame.hpp"
#include "utility/BorrowedPointer.hpp"

namespace buffer {
class ZerothPage {
public:
    ZerothPage(utils::BorrowedPointer<BufferFrame> bufferFrame) : _bufferFrame(std::move(bufferFrame)) {}

private:
    utils::BorrowedPointer<BufferFrame> _bufferFrame;
};
} // namespace buffer