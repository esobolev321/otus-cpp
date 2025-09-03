#include <gtest/gtest.h>
#include "lib.h"

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
