
#include <iostream>
#include <vector>
#include <algorithm>

void siftDown(std::vector<int>& nums, int l, int r) {
    int fa = l, ch = l * 2 + 1;
    while (ch <= r) {
        if (ch < r && nums[ch + 1] > nums[ch]) {
            ch++;
        }
        if (nums[fa] >= nums[ch]) {
            return;
        }
        std::swap(nums[fa], nums[ch]);
        fa = ch;
        ch = ch * 2 + 1;
    }

    return;
}

void heapSort(std::vector<int>& nums) {
    int n = nums.size();
    for (int i = (n - 1 - 1) / 2; i >= 0; i--) {
        siftDown(nums, i, n - 1);
    }

    for (int i = n - 1; i > 0; i--) {
        std::swap(nums[i], nums[0]);
        siftDown(nums, 0, i - 1);
    }

    return;
}

int main() {
    std::vector<int> nums{1, -1, 5, 8, 5, 2, 0, 3};
    heapSort(nums);
    for (int x : nums) {
        std::cout << x << " ";
    }
    return 0;
}