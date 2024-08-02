#include <vector>
#include <iostream>

int MooreVoting(std::vector<int>& nums) {
	int major = nums[0], cnt = 0;
	for (int x : nums) {
		if (x == major) {
			cnt++;
		}
		else {
			if (--cnt < 0) {
				major = x;
				cnt = 1;
			}
		}
	}
	return major;
}

int main() {
	std::vector<int> test1 = { 2, 2, 1, 1, 1, 2, 2 };
	std::vector<int> test2 = { 3, 3, 4, 2, 4, 4, 2, 4, 4 };
	std::vector<int> test3 = { 6, 5, 5 };

	std::cout << "Majority element in test1: " << MooreVoting(test1) << std::endl; // 2
	std::cout << "Majority element in test2: " << MooreVoting(test2) << std::endl; // 4
	std::cout << "Majority element in test3: " << MooreVoting(test3) << std::endl; // 5

	return 0;
}
