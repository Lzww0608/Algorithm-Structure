#include <iostream>
#include <vector>
#include <algorithm>

std::vector<int> longestIncreasingSequence(std::vector<int>& nums);

int main() {
	std::vector<int> nums = { 10, 9, 2, 5, 3, 7, 101, 18 };

	std::vector<int> result = longestIncreasingSequence(nums);

	std::cout << "最长递增子序列的长度: " << result.size() << std::endl;
	std::cout << "最长递增子序列: ";
	for (int num : result) {
		std::cout << num << " ";
	}
	std::cout << std::endl;

	return 0;
}

std::vector<int> longestIncreasingSequence(std::vector<int>& nums) {
	std::vector<int> a;
	for (int x : nums) {
		if (!a.empty() && a.back() >= x) {
			auto pos = std::lower_bound(a.begin(), a.end(), x);
			*pos = x;
		} else {
			a.push_back(x);
		}
	}

	return a;
}
