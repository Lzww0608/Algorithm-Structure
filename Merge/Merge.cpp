/*
给定一个数组 nums ，如果 i < j 且 nums[i] > nums[j]，
我们就将 (i, j) 称作一个逆序对。
你需要返回给定数组中的逆序对的数量。
*/
#include <vector>
#include <iostream>

int mergeSort(int l, int r, std::vector<int>& nums) {
	if (l >= r) return 0;

	int mid = l + ((r - l) >> 1);
	int cnt = mergeSort(l, mid, nums) + mergeSort(mid + 1, r, nums);

	int i = l, j = mid + 1;
	while (i <= mid) {
		while (j <= r && nums[j] < nums[i]) {
			j++;
		}
		cnt += j - mid - 1;
		i++;
	}

	std::vector<int> tmp(r - l + 1, 0);
	int k = 0;
	for (i = l, j = mid + 1; i <= mid && j <= r;) {
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

	return cnt;
}

int main() {
	//std::vector<int> nums{ 9, 7, 5, 4, 6 };
	std::vector<int> nums{7,5,6,4};
	std::cout << mergeSort(0, nums.size() - 1, nums) << std::endl;
}