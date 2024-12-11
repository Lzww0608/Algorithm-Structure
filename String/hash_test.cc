#include <gtest/gtest.h>
#include "hash.h"  

class HashTest : public ::testing::Test {
protected:
    std::string str = "teststring";
    Hash* hash;

    void SetUp() override {
        hash = new Hash(str);
    }

    void TearDown() override {
        delete hash;
    }
};

TEST_F(HashTest, InitializationTest) {
    ASSERT_TRUE(hash != nullptr);
}

TEST_F(HashTest, SubHashTest) {
    size_t l = 1, r = 4;
    uint32_t expected_hash = hash->subHash(l, r); 
    
    ASSERT_NO_THROW(hash->subHash(l, r));
}

TEST_F(HashTest, EmptySubHashTest) {
    size_t l = 3, r = 3;  
    uint32_t expected_hash = hash->subHash(l, r); 
    
    ASSERT_NO_THROW(hash->subHash(l, r));
}


TEST_F(HashTest, BoundarySubHashTest) {
    size_t l = 1, r = str.size();  
    uint32_t expected_hash = hash->subHash(l, r);
    
    ASSERT_NO_THROW(hash->subHash(l, r));
}

TEST_F(HashTest, DifferentStringTest) {
    std::string new_str = "anotherTestString";
    Hash new_hash(new_str);

    size_t l = 1, r = 6;  
    uint32_t hash_value = new_hash.subHash(l, r);

    ASSERT_NO_THROW(new_hash.subHash(l, r));
}


TEST_F(HashTest, LargeModTest) {
    uint32_t large_mod = 1e9 + 9;
    Hash large_mod_hash(str, 131, large_mod);
    
    size_t l = 1, r = 4;  
    uint32_t hash_value = large_mod_hash.subHash(l, r);

    ASSERT_NO_THROW(large_mod_hash.subHash(l, r));
}


TEST_F(HashTest, HashCorrectnessTest) {
    std::string str1 = "teststring", str2 = "sstestssting";
    Hash hash1(str1);
    Hash hash2(str2);

    size_t l1 = 0, r1 = 4, l2 = 2, r2 = 6;  
    uint32_t result = hash1.subHash(l1, r1);

    std::cout << "Calculated hash: " << result << std::endl;

    uint32_t expected_hash = hash2.subHash(l2, r2); 

    ASSERT_EQ(result, expected_hash);
}


