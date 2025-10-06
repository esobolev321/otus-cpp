#include <gtest/gtest.h>
#include "lib.h"
#include <allocator.h>
#include <my_vector.h>
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

// Test class for version function
class VersionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test version function returns non-zero
TEST_F(VersionTest, VersionReturnsNonZero) {
    EXPECT_GT(version(), 0);
}

// Test version function returns expected value
TEST_F(VersionTest, VersionReturnsExpectedValue) {
    int ver = version();
    EXPECT_TRUE(ver > 0);
    EXPECT_LT(ver, 1000000);  // Reasonable upper bound
}

// Simple test without fixture
TEST(LibTest, VersionIsPositive) {
    ASSERT_GT(version(), 0);
}

// Test with multiple assertions
TEST(LibTest, VersionProperties) {
    int ver = version();
    
    EXPECT_GT(ver, 0) << "Version should be positive";
    EXPECT_LT(ver, 1000000) << "Version should be reasonable";
    EXPECT_EQ(ver, version()) << "Version should be consistent";
}

// ===============================================
// MyAllocator Tests
// ===============================================

class MyAllocatorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MyAllocatorTest, BasicAllocation) {
    MyAllocator<int, 10> alloc;
    
    // Allocate space for 5 integers
    int* ptr = alloc.allocate(5);
    ASSERT_NE(ptr, nullptr);
    
    // Should be able to write and read data
    for (int i = 0; i < 5; ++i) {
        ptr[i] = i * 10;
    }
    
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(ptr[i], i * 10);
    }
    
    // Deallocate (should not crash)
    alloc.deallocate(ptr, 5);
}

TEST_F(MyAllocatorTest, CapacityLimits) {
    MyAllocator<int, 5> small_alloc;
    
    // Should be able to allocate up to capacity
    int* ptr1 = small_alloc.allocate(3);
    ASSERT_NE(ptr1, nullptr);
    
    int* ptr2 = small_alloc.allocate(2);
    ASSERT_NE(ptr2, nullptr);
    
    // Exceeding capacity should throw
    EXPECT_THROW(small_alloc.allocate(1), std::bad_alloc);
}

TEST_F(MyAllocatorTest, AllocatorEquality) {
    MyAllocator<int, 10> alloc1;
    MyAllocator<int, 10> alloc2(alloc1);  // Copy constructor
    MyAllocator<int, 10> alloc3;
    
    EXPECT_TRUE(alloc1 == alloc2);
    EXPECT_FALSE(alloc1 != alloc2);
    EXPECT_TRUE(alloc1 != alloc3);
    EXPECT_FALSE(alloc1 == alloc3);
}

TEST_F(MyAllocatorTest, RebindAllocator) {
    MyAllocator<int, 100> int_alloc;
    MyAllocator<double, 100> double_alloc(int_alloc);
    
    // Should share the same control block
    EXPECT_TRUE(int_alloc == double_alloc);
    
    double* d_ptr = double_alloc.allocate(5);
    ASSERT_NE(d_ptr, nullptr);
    
    for (int i = 0; i < 5; ++i) {
        d_ptr[i] = i * 3.14;
    }
    
    for (int i = 0; i < 5; ++i) {
        EXPECT_DOUBLE_EQ(d_ptr[i], i * 3.14);
    }
}

TEST_F(MyAllocatorTest, AlignmentTest) {
    // Test alignment with different allocator types
    MyAllocator<char, 100> char_alloc;
    MyAllocator<int, 100> int_alloc;
    MyAllocator<double, 100> double_alloc;
    
    char* char_ptr = char_alloc.allocate(1);
    int* int_ptr = int_alloc.allocate(1);
    double* double_ptr = double_alloc.allocate(1);
    
    ASSERT_NE(char_ptr, nullptr);
    ASSERT_NE(int_ptr, nullptr);
    ASSERT_NE(double_ptr, nullptr);
    
    // Check alignment
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(int_ptr) % alignof(int), 0);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(double_ptr) % alignof(double), 0);
}

TEST_F(MyAllocatorTest, CrossTypeAlignmentWithRebind) {
    // Use the same control block across different types
    MyAllocator<char, 128> a_char;                // capacity in elements of char
    MyAllocator<int, 128> a_int(a_char);          // shares ctrl
    MyAllocator<double, 128> a_double(a_char);    // shares ctrl

    // Allocate a few bytes first to force a misaligned "used" offset
    char* c1 = a_char.allocate(1);
    ASSERT_NE(c1, nullptr);

    // Now allocate an int and a double; both must be properly aligned
    int* ip = a_int.allocate(1);
    ASSERT_NE(ip, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ip) % alignof(int), 0u);

    // Allocate another single byte to disturb alignment again
    char* c2 = a_char.allocate(1);
    ASSERT_NE(c2, nullptr);

    double* dp = a_double.allocate(1);
    ASSERT_NE(dp, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(dp) % alignof(double), 0u);
}

// ===============================================
// MyVector Tests
// ===============================================

class MyVectorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MyVectorTest, DefaultConstruction) {
    MyVector<int> vec;
    
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 0);
    EXPECT_TRUE(vec.empty());
}

TEST_F(MyVectorTest, PushBackAndAccess) {
    MyVector<int> vec;
    
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    
    EXPECT_EQ(vec.size(), 3);
    EXPECT_GE(vec.capacity(), 3);
    EXPECT_FALSE(vec.empty());
    
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    EXPECT_EQ(vec[2], 30);
}

TEST_F(MyVectorTest, CapacityGrowth) {
    MyVector<int> vec;
    
    // Test that capacity grows as expected
    std::size_t prev_capacity = 0;
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
        EXPECT_GE(vec.capacity(), vec.size());
        if (vec.capacity() > prev_capacity) {
            // Capacity should grow by at least doubling (or start at 1)
            EXPECT_TRUE(vec.capacity() == 1 || vec.capacity() >= prev_capacity * 2);
            prev_capacity = vec.capacity();
        }
    }
}

TEST_F(MyVectorTest, Reserve) {
    MyVector<int> vec;
    
    vec.reserve(50);
    EXPECT_GE(vec.capacity(), 50);
    EXPECT_EQ(vec.size(), 0);
    
    // Add elements without triggering reallocation
    for (int i = 0; i < 50; ++i) {
        vec.push_back(i);
    }
    
    EXPECT_EQ(vec.size(), 50);
    for (int i = 0; i < 50; ++i) {
        EXPECT_EQ(vec[i], i);
    }
}

TEST_F(MyVectorTest, Clear) {
    MyVector<int> vec;
    
    for (int i = 0; i < 5; ++i) {
        vec.push_back(i);
    }
    
    EXPECT_EQ(vec.size(), 5);
    
    vec.clear();
    EXPECT_EQ(vec.size(), 0);
    EXPECT_TRUE(vec.empty());
    // Capacity should remain unchanged
    EXPECT_GT(vec.capacity(), 0);
}

TEST_F(MyVectorTest, IteratorBasic) {
    MyVector<int> vec;
    
    for (int i = 0; i < 5; ++i) {
        vec.push_back(i * 2);
    }
    
    // Test non-const iterators
    int expected = 0;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        EXPECT_EQ(*it, expected * 2);
        expected++;
    }
    
    // Test const iterators
    const MyVector<int>& const_vec = vec;
    expected = 0;
    for (auto it = const_vec.begin(); it != const_vec.end(); ++it) {
        EXPECT_EQ(*it, expected * 2);
        expected++;
    }
}

TEST_F(MyVectorTest, RangeBasedFor) {
    MyVector<int> vec;
    
    for (int i = 0; i < 5; ++i) {
        vec.push_back(i * 3);
    }
    
    int expected = 0;
    for (const auto& value : vec) {
        EXPECT_EQ(value, expected * 3);
        expected++;
    }
}

TEST_F(MyVectorTest, WithCustomAllocator) {
    // Use a larger allocator capacity to account for vector growth
    // Vector doubles capacity, so for 20 elements we need: 1->2->4->8->16->32
    MyVector<int, MyAllocator<int, 100>> vec;
    
    for (int i = 0; i < 20; ++i) {
        vec.push_back(i);
    }
    
    EXPECT_EQ(vec.size(), 20);
    for (int i = 0; i < 20; ++i) {
        EXPECT_EQ(vec[i], i);
    }
}

TEST_F(MyVectorTest, CustomAllocatorLimits) {
    // Test with very small allocator to verify proper exception handling
    MyVector<int, MyAllocator<int, 5>> small_vec;
    
    // Should be able to add a few elements
    small_vec.push_back(1);
    small_vec.push_back(2);
    EXPECT_EQ(small_vec.size(), 2);
    
    // Adding more should eventually hit allocator limits and throw
    EXPECT_THROW({
        for (int i = 0; i < 10; ++i) {
            small_vec.push_back(i);
        }
    }, std::bad_alloc);
}

TEST_F(MyVectorTest, CustomAllocatorWithReserve) {
    // Test using reserve to avoid reallocation with custom allocator
    MyVector<int, MyAllocator<int, 30>> vec;
    
    // Reserve exact capacity to avoid growth
    vec.reserve(25);
    
    // Fill up to reserved capacity
    for (int i = 0; i < 25; ++i) {
        vec.push_back(i * 2);
    }
    
    EXPECT_EQ(vec.size(), 25);
    EXPECT_GE(vec.capacity(), 25);
    
    for (int i = 0; i < 25; ++i) {
        EXPECT_EQ(vec[i], i * 2);
    }
}

TEST_F(MyVectorTest, StringType) {
    MyVector<std::string> vec;
    
    vec.push_back("Hello");
    vec.push_back("World");
    vec.push_back("!");
    
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], "Hello");
    EXPECT_EQ(vec[1], "World");
    EXPECT_EQ(vec[2], "!");
}

// ===============================================
// Integration Tests
// ===============================================

TEST(IntegrationTest, VectorWithMyAllocator) {
    MyVector<int, MyAllocator<int, 200>> vec;
    
    // Fill vector with custom allocator
    for (int i = 0; i < 50; ++i) {
        vec.push_back(i * i);
    }
    
    EXPECT_EQ(vec.size(), 50);
    
    // Verify all values
    for (int i = 0; i < 50; ++i) {
        EXPECT_EQ(vec[i], i * i);
    }
    
    // Test iteration
    int count = 0;
    for (const auto& value : vec) {
        EXPECT_EQ(value, count * count);
        count++;
    }
}

TEST(IntegrationTest, AllocatorStressTest) {
    MyAllocator<int, 1000> alloc;
    
    std::vector<int*> ptrs;
    
    // Allocate many small chunks
    for (int i = 0; i < 100; ++i) {
        int* ptr = alloc.allocate(1);
        ASSERT_NE(ptr, nullptr);
        *ptr = i;
        ptrs.push_back(ptr);
    }
    
    // Verify all values
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(*ptrs[i], i);
    }
    
    // Deallocate all
    for (int i = 0; i < 100; ++i) {
        alloc.deallocate(ptrs[i], 1);
    }
}
