#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <algorithm>

void siftDown(std::vector<int>& nums, int l, int r) {
	int pa = l, ch = l * 2 + 1;
	while (ch <= r) {
		if (ch < r && nums[ch+1] > nums[ch]) ch++;
		if (nums[pa] >= nums[ch]) return;
		std::swap(nums[pa], nums[ch]);
		pa = ch;
		ch = ch * 2 + 1;
	}
}

int main() {
	std::vector<int> nums{1, -1, 5, 8, 5, 2, 0, 3};
	int n = nums.size();
	for (int i = (n - 1 - 1) / 1; i >= 0; i--) {
		siftDown(nums, i, n - 1);
	}
	for (int i = n - 1; i > 0; i--) {
		std::swap(nums[i], nums[0]);
		siftDown(nums, 0, i - 1);
	}
	for (int x : nums) {
		std::cout << x << " ";
	}
}