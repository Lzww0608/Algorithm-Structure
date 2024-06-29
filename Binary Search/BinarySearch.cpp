/*
 二分搜寻法（搜寻原则的代表）
 说明如果搜寻的数列已经有排序，应该尽量利用它们已排序的特性，以减少搜寻比对的次数，
 这是搜寻的基本原则，二分搜寻法是这个基本原则的代表。
 解法在二分搜寻法中，从数列的中间开始搜寻，如果这个数小于我们所搜寻的数，由于数列
 已排序，则该数左边的数一定都小于要搜寻的对象，所以无需浪费时间在左边的数；如果搜寻
 的数大于所搜寻的对象，则右边的数无需再搜寻，直接搜寻左边的数。
 所以在二分搜寻法中，将数列不断的分为两个部份，每次从分割的部份中取中间数比对，例如
 要搜寻92于以下的数列，首先中间数索引为(0+9)/2 = 4（索引由0开始）：
 [3 24 57 57 67 68 83 90 92 95]
 由于67小于92，所以转搜寻右边的数列：
 3 24 57 57 67 [68 83 90 92 95]
 由于90小于92，再搜寻右边的数列，这次就找到所要的数了：
 3 24 57 57 67 68 83 90 [92 95]
*/
#include <iostream>
#include <vector>
#include <algorithm>

// 左闭右开区间
int binarySearch1(std::vector<int> &nums, int target) {
	int l = 0, r = nums.size();
	while (l < r) {
		int mid = l + ((r - l) >> 1); // 防止溢出
		// int mid = r - ((r - l) >> 1); 
		if (nums[mid] == target) {
			return mid;
		} else if (nums[mid] > target) {
			r = mid;
		} else {
			l = mid + 1;
		}
	}
	return - 1; // 未找到
}

// 闭区间
int binarySearch2(std::vector<int> &nums, int target) {
	int l = 0, r = nums.size() - 1; //注意：数组长度不能为0，oveflow
	while (l <= r) {
		int mid = l + ((r - l) >> 1); // 防止溢出
		// int mid = r - ((r - l) >> 1); 
		if (nums[mid] == target) {
			return mid;
		}
		else if (nums[mid] > target) {
			r = mid - 1;
		}
		else {
			l = mid + 1;
		}
	}
	return -1; // 未找到
}

// 开区间
int binarySearch3(std::vector<int> &nums, int target) {
	int l = -1, r = nums.size();
	while (l + 1 < r) {
		int mid = l + ((r - l) >> 1); // 防止溢出
		// int mid = r - ((r - l) >> 1); 
		if (nums[mid] == target) {
			return mid;
		}
		else if (nums[mid] > target) {
			r = mid;
		}
		else {
			l = mid;
		}
	}
	return -1; // 未找到
}


int main() {
	std::vector<int> nums{3, 24, 57, 59, 67, 68, 83, 90, 92, 95};
	// std::sort(nums.begin(), nums.end());
	std::cout << binarySearch1(nums, 67) << std::endl;
	std::cout << binarySearch2(nums, 67) << std::endl;
	std::cout << binarySearch3(nums, 67) << std::endl;

	std::cout << binarySearch1(nums, 66) << std::endl;
	std::cout << binarySearch2(nums, 66) << std::endl;
	std::cout << binarySearch3(nums, 66) << std::endl;

	return 0;
}