#include <gtest/gtest.h>

// Basic test to verify gtest is working
TEST(BasicTest, GTestIsWorking) {
    EXPECT_EQ(1 + 1, 2);
}

// Example test for your code
TEST(IdentitySessionIdTest, DefaultConstructor) {
    IdentitySessionId id;
    // Verify all bytes are initialized to 0
    for (size_t i = 0; i < crypto_hash_sha256_BYTES; i++) {
        EXPECT_EQ(id.data[i], 0);
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 