#pragma once

#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class MyVector {
private:
    using AllocTraits = std::allocator_traits<Allocator>;
    
    Allocator alloc;
    T* data;
    std::size_t _size;
    std::size_t _capacity;

public:
    explicit MyVector(const Allocator& alloc = Allocator())
        : alloc(alloc), data(nullptr), _size(0), _capacity(0) {}

    ~MyVector() {
        clear();
        if (data) {
            alloc.deallocate(data, _capacity);
        }
    }
    
    MyVector(const MyVector&) = delete;
    MyVector& operator=(const MyVector&) = delete;

    T& operator[](std::size_t pos) { return data[pos]; }
    const T& operator[](std::size_t pos) const { return data[pos]; }
    
    std::size_t size() const { return _size; }
    std::size_t capacity() const { return _capacity; }
    bool empty() const { return _size == 0; }

    void push_back(const T& value) {
        if (_size == _capacity) {
            reserve(_capacity == 0 ? 1 : _capacity * 2);
        }
        AllocTraits::construct(alloc, data + _size, value);
        ++_size;
    }
    
    void reserve(std::size_t new_capacity) {
        if (new_capacity <= _capacity) return;
        T* new_data = alloc.allocate(new_capacity);
        
        for (std::size_t i = 0; i < _size; ++i) {
            AllocTraits::construct(alloc, new_data + i, data[i]); 
            AllocTraits::destroy(alloc, data + i);
        }
        
        if (data) {
            alloc.deallocate(data, _capacity);
        }
        
        data = new_data;
        _capacity = new_capacity;
    }
    
    void clear() {
        for (std::size_t i = 0; i < _size; ++i) {
            AllocTraits::destroy(alloc, data + i);
        }
        _size = 0;
    }

    T* begin() { return data; }
    T* end() { return data + _size; }
    const T* begin() const { return data; }
    const T* end() const { return data + _size; }
};