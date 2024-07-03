#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>

void quickSort(std::vector<int>& nums, int l, int r) {
	if (l >= r) return;

	int mid = l + ((r - l) >> 1);
	int pivot = nums[l + std::rand() % (r - l + 1)];
	int i = l, j = l, k = r + 1;
	while (i < k) {
		if (nums[i] > pivot) {
			std::swap(nums[i], nums[--k]);
		} else if (nums[i] < pivot) {
			std::swap(nums[i++], nums[j++]);
		} else {
			i++;
		}
	}

	quickSort(nums, l, j - 1);
	quickSort(nums, k, r);
}

int main() {
	std::vector<int> nums{1, -1, 5, 8, 5, 2, 0, 3};
	quickSort(nums, 0, nums.size() - 1); // nums.size() > 0
	for (int x : nums) {
		std::cout << x << " ";
	}
}