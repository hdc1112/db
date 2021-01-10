#pragma once

#include "buffer/BufferFrame.hpp"

namespace buffer {
class ZerothPage {
public:
    ZerothPage(BufferFrame bufferFrame) : _bufferFrame(std::move(bufferFrame)) {}

private:
    BufferFrame _bufferFrame;
};
} // namespace buffer