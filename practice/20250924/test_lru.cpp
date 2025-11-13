#include <gtest/gtest.h>
#include "lru.cpp"

class LRUCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试用例开始前的设置
    }

    void TearDown() override {
        // 每个测试用例结束后的清理
    }
};

// 测试基本的get和put操作
TEST_F(LRUCacheTest, BasicGetPut) {
    LRUCache cache(2);
    
    // 测试空缓存的get操作
    EXPECT_EQ(cache.get(1), -1);
    
    // 测试put和get操作
    cache.put(1, 1);
    EXPECT_EQ(cache.get(1), 1);
    
    cache.put(2, 2);
    EXPECT_EQ(cache.get(1), 1);
    EXPECT_EQ(cache.get(2), 2);
}

// 测试缓存容量超限时的LRU淘汰机制
TEST_F(LRUCacheTest, CapacityExceeded) {
    LRUCache cache(2);
    
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3); // 应该淘汰key=1
    
    EXPECT_EQ(cache.get(1), -1); // key=1应该被淘汰
    EXPECT_EQ(cache.get(2), 2);
    EXPECT_EQ(cache.get(3), 3);
}

// 测试访问操作会更新元素的访问顺序
TEST_F(LRUCacheTest, AccessUpdatesOrder) {
    LRUCache cache(2);
    
    cache.put(1, 1);
    cache.put(2, 2);
    cache.get(1); // 访问key=1，使其变为最近使用
    cache.put(3, 3); // 应该淘汰key=2而不是key=1
    
    EXPECT_EQ(cache.get(1), 1); // key=1应该还在
    EXPECT_EQ(cache.get(2), -1); // key=2应该被淘汰
    EXPECT_EQ(cache.get(3), 3);
}

// 测试更新已存在的key
TEST_F(LRUCacheTest, UpdateExistingKey) {
    LRUCache cache(2);
    
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(1, 10); // 更新key=1的值
    
    EXPECT_EQ(cache.get(1), 10); // 值应该被更新
    
    cache.put(3, 3); // 应该淘汰key=2
    EXPECT_EQ(cache.get(1), 10); // key=1应该还在
    EXPECT_EQ(cache.get(2), -1); // key=2应该被淘汰
    EXPECT_EQ(cache.get(3), 3);
}

// 测试单个容量的缓存
TEST_F(LRUCacheTest, SingleCapacity) {
    LRUCache cache(1);
    
    cache.put(1, 1);
    EXPECT_EQ(cache.get(1), 1);
    
    cache.put(2, 2); // 应该淘汰key=1
    EXPECT_EQ(cache.get(1), -1);
    EXPECT_EQ(cache.get(2), 2);
}

// 测试复杂的混合操作序列
TEST_F(LRUCacheTest, ComplexOperationSequence) {
    LRUCache cache(3);
    
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    cache.put(4, 4); // 淘汰key=1
    
    EXPECT_EQ(cache.get(4), 4);
    EXPECT_EQ(cache.get(3), 3);
    EXPECT_EQ(cache.get(2), 2);
    EXPECT_EQ(cache.get(1), -1); // key=1被淘汰
    
    cache.put(5, 5); // 淘汰key=4 (因为4是最久未访问的)
    EXPECT_EQ(cache.get(5), 5);
    EXPECT_EQ(cache.get(3), 3);
    EXPECT_EQ(cache.get(2), 2);
    EXPECT_EQ(cache.get(4), -1); // key=4被淘汰
}

// 测试边界情况：连续相同的操作
TEST_F(LRUCacheTest, RepeatedOperations) {
    LRUCache cache(2);
    
    cache.put(1, 1);
    cache.put(1, 1); // 重复put相同的key-value
    EXPECT_EQ(cache.get(1), 1);
    
    cache.get(1); // 重复get
    cache.get(1);
    EXPECT_EQ(cache.get(1), 1);
    
    cache.put(2, 2);
    cache.put(3, 3); // 应该淘汰key=1
    EXPECT_EQ(cache.get(1), -1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
