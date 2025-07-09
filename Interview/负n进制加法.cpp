// 实现负三进制的加法（可拓展至负n进制）

#include <iostream>
#include <algorithm>
#include <vector>


std::vector<int> addNeg(std::vector<int>& a, std::vector<int>& b) {
	int i = a.size() - 1,j = b.size() - 1;
	std::vector<int> ans;
	for (int add = 0; add != 0 || i >= 0 || j >= 0; i--, j--) {
		int x = i >= 0 ? a[i] : 0;
		int y = j >= 0 ? b[j] : 0;
		auto tmp = x + y + add;
		add = 0;

		if (tmp >= 3) {
			tmp -= 3;
			add--;
		} else if (tmp < 0) {
			tmp += 3;
			add++;
		}

		ans.push_back(tmp);
	}
	while (ans.size() > 1 && ans.back() == 0) {
		ans.pop_back();
	}

	std::reverse(ans.begin(), ans.end());

	return ans;
}

void main() {
	std::vector<int> a = { 1, 0, 2, 1 };
	std::vector<int> b = { 1, 1, 1 };
	auto c = addNeg(a, b);
	for (auto x : c) {
		std::cout << x << " ";
	}
	std::cout << std::endl;
}