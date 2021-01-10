#pragma once

#include "Assert.hpp"
#include "BuildFlagsGenerated.hpp"
#include "utility/ClassUtility.hpp"

#include <atomic>
#include <typeinfo>

namespace utils {

template<typename T>
class BorrowableObject;
template<typename T>
class BorrowedPointer {
public:
    BorrowedPointer() = default;
    explicit BorrowedPointer(T* pointer) : _pointer(pointer) {}
    BorrowedPointer(const BorrowedPointer& other) noexcept {
        if (other._pointer != nullptr) {
            _pointer = other._pointer;
            borrowable()->increment();
        }
    }
    BorrowedPointer(BorrowedPointer&& other) noexcept {
        _pointer = other._pointer;
        other._pointer = nullptr;
    }

    BorrowedPointer& operator=(const BorrowedPointer& other) noexcept {
        if (this != &other) {
            if (other._pointer != nullptr) {
                bool isNull = (_pointer == nullptr);
                _pointer = other._pointer;
                if (isNull) {
                    borrowable()->increment();
                }
            } else {
                reset();
            }
        }
        return *this;
    }
    BorrowedPointer& operator=(BorrowedPointer&& other) noexcept {
        if (this != &other) {
            if (_pointer != nullptr && other._pointer != nullptr) {
                _pointer = other._pointer;
                other.reset();
            } else if (_pointer == nullptr && other._pointer != nullptr) {
                _pointer = other._pointer;
                other._pointer = nullptr;
            } else if (_pointer != nullptr && other._pointer == nullptr) {
                _pointer = other._pointer;
                reset();
            }
        }
        return *this;
    }

    T* get() const noexcept {
        return _pointer;
    }

    void reset() {
        if (_pointer != nullptr) {
            borrowable()->release();
            _pointer = nullptr;
        }
    }

    BorrowableObject<T>* borrowable() noexcept {
        return static_cast<BorrowableObject<T>*>(_pointer);
    }

    explicit operator bool() const noexcept {
        return _pointer != nullptr;
    }

    T& operator*() const noexcept {
        return *_pointer;
    }

    T* operator->() const noexcept {
        return _pointer;
    }

    ~BorrowedPointer() {
        reset();
    }

    bool operator==(const BorrowedPointer& other) const {
        return _pointer == other._pointer;
    }

    bool operator!=(const BorrowedPointer& other) const {
        return _pointer != other._pointer;
    }

    bool operator<(const BorrowedPointer& other) const {
        return _pointer < other._pointer;
    }

    bool operator<=(const BorrowedPointer& other) const {
        return _pointer <= other._pointer;
    }

    bool operator>(const BorrowedPointer& other) const {
        return _pointer > other._pointer;
    }

    bool operator>=(const BorrowedPointer& other) const {
        return _pointer >= other._pointer;
    }

    bool operator==(const void* other) const {
        return _pointer == other;
    }

    bool operator!=(const void* other) const {
        return !(*this == other);
    }

private:
    T* _pointer;
};

template<bool enabled>
class BorrowedCounter {
public:
    void inc() {
        ++_counter;
    }
    void dec() {
        --_counter;
    }
    int count() {
        return _counter;
    }

private:
    std::atomic<int> _counter{0};
};
template<>
class BorrowedCounter<false> {
public:
    void inc() {}
    void dec() {}
    int count() {
        return 0;
    }
};
template<typename T>
class BorrowableObject : private BorrowedCounter<kBorrowedCounterCheck> {
public:
    BorrowableObject() = default;

    BorrowedPointer<T> borrow() {
        inc();
        return BorrowedPointer<T>(static_cast<T*>(this));
    }

    BorrowableObject(const BorrowableObject& /*unused*/) noexcept {}
    BorrowableObject(BorrowableObject&& /*unused*/) noexcept {}
    BorrowableObject& operator=(const BorrowableObject& /*unused*/) noexcept {}
    BorrowableObject& operator=(BorrowableObject&& /*unused*/) noexcept {}

    void increment() {
        inc();
    }

    void release() {
        dec();
    }

    int borrowCount() {
        return count();
    }

    ~BorrowableObject() {
        DEBUG_ASSERT(count() == 0, "For {}, {} BorrowedPointers are not returned", __class__, count());
    }
};
} // namespace utils