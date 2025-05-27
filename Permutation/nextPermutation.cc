#include <iostream>
#include <vector>
#include <gtest/gtest.h>


bool nextPermutation(std::vector<int>& nums) {
    int i = nums.size() - 2;
    while (i >= 0 && nums[i] >= nums[i + 1]) {
        i--;
    }
    if (i < 0) {
        return false;
    }

    int j = nums.size() - 1;
    while (j >= 0 && nums[j] <= nums[i]) {
        j--;
    }

    std::swap(nums[i], nums[j]);
    std::reverse(nums.begin() + i + 1, nums.end());
    return true;
}


TEST(NextPermutationTest, BasicTest) {
    std::vector<int> nums = {1, 2, 3};
    std::vector<std::vector<int>> expected = {
        {1, 2, 3},
        {1, 3, 2},
        {2, 1, 3},
        {2, 3, 1},
        {3, 1, 2},
        {3, 2, 1}
    };
    
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(nums, expected[i]);
        if (i < expected.size() - 1) {
            EXPECT_TRUE(nextPermutation(nums));
        } else {
            EXPECT_FALSE(nextPermutation(nums));
        }
    }
}

TEST(NextPermutationTest, DuplicateElements) {
    std::vector<int> nums = {1, 1, 2};
    std::vector<std::vector<int>> expected = {
        {1, 1, 2},
        {1, 2, 1},
        {2, 1, 1}
    };
    
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(nums, expected[i]);
        if (i < expected.size() - 1) {
            EXPECT_TRUE(nextPermutation(nums));
        } else {
            EXPECT_FALSE(nextPermutation(nums));
        }
    }
}

TEST(NextPermutationTest, SingleElement) {
    std::vector<int> nums = {1};
    EXPECT_FALSE(nextPermutation(nums));
    EXPECT_EQ(nums, std::vector<int>{1});
}

TEST(NextPermutationTest, EmptyVector) {
    std::vector<int> nums;
    EXPECT_FALSE(nextPermutation(nums));
    EXPECT_TRUE(nums.empty());
}

