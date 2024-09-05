/*
Given an integer array nums, reorder it such that nums[0] <= nums[1] >= nums[2] <= nums[3]....
*/

#include <iostream>
#include <vector>


void wiggleSort(std::vector<int> &arr) {
	int n = arr.size();
	for (int i = 1; i < n; ++i) {
		if (i % 2 == 1 && arr[i] < arr[i - 1] || i % 2 == 0 && arr[i] > arr[i - 1]) {
			std::swap(arr[i], arr[i-1]);
		}
	}

	return;
}

int main() {
	std::vector<int> arr {3, 5, 2, 1, 6, 4};
	wiggleSort(arr);
	int n = arr.size();
	for (int i = 0; i < n; ++i) {
		std::cout << arr[i] << " \n"[i == n-1];
	}
}