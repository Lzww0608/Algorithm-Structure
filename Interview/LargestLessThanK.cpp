//题目：给定一个包含1-9中任意多个不同数字的数组，输入一个数字x，求由给定数组中的数字组成的小于x的最大整数是多少（数组中的数字可以重复使用）
//注：假定x不超过MAX_INT且结果一定存在， 如果超过结果需要改为用string表示
// a = {1, 2, 9, 4} x = 2533, target = 2499
// a = {9}, x = 111, target = 99
// a = {2, 5, 1}, x = 252, target = 251
// a = {2, 5}, x = 252, target = 225
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

// find the largest number that is less than target
int find(vector<int>& a, int target) {
	for (int i = a.size() - 1; i >= 0; --i)
		if (a[i] < target)
			return a[i];

	return 0;
}


int largestLessThanK(vector<int>& a, int x) {
	vector<int> digits;
	while (x > 0) {
		digits.push_back(x % 10);
		x /= 10;
	}
	reverse(digits.begin(), digits.end());
	sort(a.begin(), a.end());
	//use a hash table to record
	unordered_map<int, bool> m;
	for (auto v : a)
		m[v] = true;

	// the possible answer
	int n = digits.size();
	vector<int> res(n, 0);

	
	for (int i = 0; i < n; i++) {

		if (i < n - 1 && m.count(digits[i])) {
			res[i] = digits[i];
			continue;
		}
		//we find a number less than x and they have the same length 
		int num = find(a, digits[i]);
		if (num > 0) {
			res[i] = num;
			break;
		}
		//the least number in a is equal or larger than x if they have the same length
		// we need to backtrack


		// we should ensure i >= 0 to construct the res
		for (i = max(i - 1, 0); i >= 0; i--) {
			// try to find a number less than x 
			num = find(a, digits[i]);
			if (num > 0) {
				res[i] = num;
				for (int j = i + 1; j < res.size(); ++j)
					res[j] = 0;
				break;
			}
			// the least number in a is larger than x if they have the same length
			// we need a number shorter than x
			if (i == 0)
				res.resize(n - 1, a.back());

		}
		break;
	}

	int target = 0;
	for (int i = 0; i < res.size(); i++) {
		target *= 10;
		if (res[i] > 0) {
			target += res[i];
		}
		else {
			target += a.back();
		}
	}

	return target;
}


int main() {
	vector<int> a;
	string line;
	std::getline(std::cin, line);
	std::istringstream iss(line);
	int num;
	while (iss >> num)
		a.push_back(num);

	int x;
	cin >> x;
	cout << largestLessThanK(a, x) << endl;
	return 0;
}
/*
1 2 4 9
2533

2 5 1
252

2 5
225

9
12353

9 8
1111

1 9
9911
*/

