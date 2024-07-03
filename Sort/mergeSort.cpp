#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <algorithm>

void mergeSort(std::vector<int>& nums, int l, int r) {
	if (l >= r) return;

	int mid = l + ((r - l) >> 1);
	mergeSort(nums, l, mid);
	mergeSort(nums, mid + 1, r);
	int i = l, j = mid + 1, k = 0;
	std::vector<int> tmp(r - l + 1, 0);
	while (i <= mid && j <= r) {
		// stable sort
		if (nums[j] < nums[i]) {
			tmp[k++] = nums[j++];
		} else {
			tmp[k++] = nums[i++];
		}
	}

	while (i <= mid) {
		tmp[k++] = nums[i++];
	}

	while (j <= r) {
		tmp[k++] = nums[j++];
	}

	std::copy(tmp.begin(), tmp.end(), nums.begin() + l);
	return;
}

int main() {
	std::vector<int> nums{1, -1, 5, 8, 5, 2, 0, 3};
	mergeSort(nums, 0, nums.size() - 1); // nums.size() > 0
	for (int x : nums) {
		std::cout << x << " ";
	}
}