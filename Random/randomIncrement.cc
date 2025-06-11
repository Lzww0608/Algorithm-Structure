#include <vector>
#include <iostream>
#include <random>
#include <gtest/gtest.h>


std::vector<int> randomIncrementArray(int size, int start_val, int max_increment) {
    if (size <= 0 || max_increment <= 0) {
        return {};
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, max_increment);

    std::vector<int> arr(size);
    arr[0] = start_val;
    
    for (int i = 1; i < size; i++) {
        arr[i] = arr[i - 1] + dis(gen);
    }

    return arr;
}

// 测试边界条件
TEST(RandomIncrementArrayTest, EdgeCases) {
    // 测试size为0的情况
    auto result = randomIncrementArray(0, 10, 5);
    EXPECT_TRUE(result.empty());
    
    // 测试size为负数的情况
    result = randomIncrementArray(-1, 10, 5);
    EXPECT_TRUE(result.empty());
    
    // 测试max_increment为0的情况
    result = randomIncrementArray(5, 10, 0);
    EXPECT_TRUE(result.empty());
    
    // 测试max_increment为负数的情况
    result = randomIncrementArray(5, 10, -1);
    EXPECT_TRUE(result.empty());
}

// 测试基本功能
TEST(RandomIncrementArrayTest, BasicFunctionality) {
    int size = 10;
    int start_val = 100;
    int max_increment = 5;
    
    auto result = randomIncrementArray(size, start_val, max_increment);
    
    // 检查数组大小
    EXPECT_EQ(result.size(), size);
    
    // 检查第一个元素
    EXPECT_EQ(result[0], start_val);
    
    // 检查数组是否严格递增
    for (int i = 1; i < size; i++) {
        EXPECT_GT(result[i], result[i-1]);
        // 检查增量是否在合理范围内
        int increment = result[i] - result[i-1];
        EXPECT_GE(increment, 1);
        EXPECT_LE(increment, max_increment);
    }
}

// 测试单个元素数组
TEST(RandomIncrementArrayTest, SingleElement) {
    auto result = randomIncrementArray(1, 42, 10);
    
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 42);
}

// 测试不同的起始值
TEST(RandomIncrementArrayTest, DifferentStartValues) {
    // 测试正数起始值
    auto result1 = randomIncrementArray(5, 100, 3);
    EXPECT_EQ(result1[0], 100);
    
    // 测试负数起始值
    auto result2 = randomIncrementArray(5, -50, 3);
    EXPECT_EQ(result2[0], -50);
    
    // 测试零起始值
    auto result3 = randomIncrementArray(5, 0, 3);
    EXPECT_EQ(result3[0], 0);
}

// 测试大数组
TEST(RandomIncrementArrayTest, LargeArray) {
    int size = 1000;
    int start_val = 0;
    int max_increment = 10;
    
    auto result = randomIncrementArray(size, start_val, max_increment);
    
    EXPECT_EQ(result.size(), size);
    EXPECT_EQ(result[0], start_val);
    
    // 验证所有元素都是递增的
    for (int i = 1; i < size; i++) {
        EXPECT_GT(result[i], result[i-1]);
        int increment = result[i] - result[i-1];
        EXPECT_GE(increment, 1);
        EXPECT_LE(increment, max_increment);
    }
}

// 测试随机性（多次调用应该产生不同结果）
TEST(RandomIncrementArrayTest, Randomness) {
    int size = 10;
    int start_val = 0;
    int max_increment = 5;
    
    auto result1 = randomIncrementArray(size, start_val, max_increment);
    auto result2 = randomIncrementArray(size, start_val, max_increment);
    
    // 两次调用应该产生不同的结果（除了第一个元素）
    bool different = false;
    for (int i = 1; i < size; i++) {
        if (result1[i] != result2[i]) {
            different = true;
            break;
        }
    }
    
    // 由于是随机生成，应该有很高概率产生不同结果
    // 注意：这个测试有极小概率失败（如果两次生成完全相同）
    EXPECT_TRUE(different);
}

// 测试最大增量为1的特殊情况
TEST(RandomIncrementArrayTest, MaxIncrementOne) {
    int size = 5;
    int start_val = 10;
    int max_increment = 1;
    
    auto result = randomIncrementArray(size, start_val, max_increment);
    
    EXPECT_EQ(result.size(), size);
    EXPECT_EQ(result[0], start_val);
    
    // 当max_increment为1时，每个增量都应该是1
    for (int i = 1; i < size; i++) {
        EXPECT_EQ(result[i], result[i-1] + 1);
    }
}
