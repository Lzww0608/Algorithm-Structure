#include <iostream>
#include <vector>
#include <algorithm>


void countingSort(std::vector<int> &nums, int exp) {
	int n = nums.size();
	std::vector<int> output(n), cnt(10, 0);

	for (int x : nums) {
		int digit = (x / exp) % 10;
		cnt[digit]++;
	}

	for (int i = 1; i < 10; i++) {
		cnt[i] += cnt[i-1];
	}

	for (int i = n - 1; i >= 0; --i) {
		int digit = (nums[i] / exp) % 10;
		output[cnt[digit]-1] = nums[i];
		cnt[digit]--;
	}

	std::copy(output.begin(), output.end(), nums.begin());
}








void radixSort(std::vector<int> &nums) {
	int maxVal = *std::max_element(nums.begin(), nums.end());

	for (int exp = 1; exp <= maxVal; exp *= 10) {
		countingSort(nums, exp);
	}

}

int main() {
	std::vector<int> nums = { 170, 45, 75, 90, 802, 24, 2, 66 };
	radixSort(nums);

	for (int x : nums) {
		std::cout << x << " ";
	}
}