#pragma once
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>

struct MyAllocatorControl {
    char* base = nullptr;
    std::size_t used = 0;
    std::size_t capacity = 0;

    explicit MyAllocatorControl(std::size_t cap_bytes)
        : base(static_cast<char*>(::operator new(cap_bytes)))
        , used(0)
        , capacity(cap_bytes) {}

    ~MyAllocatorControl() { ::operator delete(base); }

    static constexpr std::size_t callculateAlign(std::size_t x, std::size_t a) noexcept {
        return (x + (a - 1)) & ~(a - 1);
    }

    template<class T>
    T* allocate(std::size_t n) {
        const std::size_t need = n * sizeof(T);
        const std::size_t aligned = callculateAlign(used, alignof(T));
        if (aligned > capacity || capacity - aligned < need)
            throw std::bad_alloc();

        T* ptr = reinterpret_cast<T*>(base + aligned);
        used = aligned + need;
        return ptr;
    }
};

template<class T, std::size_t CapacityElems = 100>
class MyAllocator {
    template<class, std::size_t> friend class MyAllocator;

    std::shared_ptr<MyAllocatorControl> ctrl;

public:
    using value_type = T;

    template<class U>
    struct rebind { using other = MyAllocator<U, CapacityElems>; };

    MyAllocator()
        : ctrl(std::make_shared<MyAllocatorControl>(CapacityElems * sizeof(T))) {}

    MyAllocator(const MyAllocator&) noexcept = default;
    MyAllocator& operator=(const MyAllocator&) noexcept = default;

    template<class U>
    MyAllocator(const MyAllocator<U, CapacityElems>& other) noexcept
        : ctrl(other.ctrl) {}

    T* allocate(std::size_t n) {
        return ctrl->allocate<T>(n);
    }

    void deallocate(T*, std::size_t) noexcept {
    }

    template<class U>
    bool operator==(const MyAllocator<U, CapacityElems>& rhs) const noexcept {
        return ctrl.get() == rhs.ctrl.get();
    }
    template<class U>
    bool operator!=(const MyAllocator<U, CapacityElems>& rhs) const noexcept {
        return !(*this == rhs);
    }
};
