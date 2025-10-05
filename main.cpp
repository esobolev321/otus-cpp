#include "lib.h"

#include <iostream>
#include <vector>
#include <map>

#include <allocator.h>
#include <my_vector.h>

const size_t CAPACITY_ELEMS = 100;
using MapAlloc = MyAllocator<std::pair<const int, int>, CAPACITY_ELEMS>;
using VectorAlloc = MyAllocator<int, CAPACITY_ELEMS>;

template<typename K, typename V, typename Cmp = std::less<K>, typename Alloc = std::allocator<std::pair<const K, V>>>
void printMap(const std::map<K, V, Cmp, Alloc>& data) {
    std::cout << "Map contents: ";
    for (const auto& pair : data) {
        std::cout << "[" << pair.first << ":" << pair.second << "] ";
    }
    std::cout << std::endl;
}

template<typename T, typename Alloc = std::allocator<T>>
void printVector(const MyVector<T, Alloc>& data) {
    std::cout << "Vector contents: ";
    for (auto it = data.begin(); it != data.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

template<typename K, typename V, typename Cmp = std::less<K>, typename Alloc = std::allocator<std::pair<const K, V>>>
void fillMap(std::map<K, V, Cmp, Alloc>& data) {
    int lastValue = 1;
    for (int i = 1; i <= 10; ++i) {
            data[i-1] = lastValue;
            lastValue *= i;
        }
}

template<typename T, typename Alloc = std::allocator<T>>
void fillVector(MyVector<T, Alloc>& data) {
    int lastValue = 1;
    for (int i = 1; i <= 10; ++i) {
        data.push_back(lastValue);
        lastValue *= i;
    }
}

int main(int, char **)
{
    {
        std::cout << "=== Testing std::map with std::allocator ===\n";
        std::map<int, int> data{};
        
        std::cout << "Adding elements to map...\n";
        fillMap(data);
        printMap(data);
        
        std::cout << std::endl;
    }
    {
        std::cout << "=== Testing std::map with custom allocator ===\n";
        std::map<int, int, std::less<int>, MapAlloc> data{};
        
        std::cout << "Adding elements to map...\n";
        fillMap(data);
        printMap(data);

        std::cout << std::endl;
    }
    {
        std::cout << "=== Testing MyVector with std::allocator ===\n";
        MyVector<int> stdVector{};
        
        std::cout << "Adding elements to vector...\n";
        fillVector(stdVector);
        printVector(stdVector);
        std::cout << std::endl;
    }
    {
        std::cout << "=== Testing MyVector with custom allocator ===\n";
        MyVector<int, VectorAlloc> customVector{};
        
        std::cout << "Adding elements to vector...\n";
        fillVector(customVector);
        printVector(customVector);
        std::cout << std::endl;
    }

    return 0;
}
