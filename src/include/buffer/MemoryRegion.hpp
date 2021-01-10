#pragma once

#include "Assert.hpp"
#include "SizeTypes.hpp"
#include "utility/BorrowedPointer.hpp"

namespace buffer {
class MemoryRegion : public utils::BorrowableObject<MemoryRegion> {
public:
    explicit MemoryRegion(std::size_t bytes) {
        DEBUG_ASSERT(bytes <= 50_MB, "bytes {} out of reasonable range", bytes);
        _memory = std::vector<uint8_t>(bytes);
    }

    void* address() {
        return _memory.data();
    }

    const void* address() const {
        return _memory.data();
    }

private:
    std::vector<uint8_t> _memory;
};
} // namespace buffer