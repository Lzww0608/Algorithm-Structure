//二十年祖传面试题：给定一个整数n,求比n大的且相邻位不相等的最小整数。
#include <iostream>
#include <unordered_map>
#include <vector>
#include <utility>
#include <map>
#include <string>
using ll = long long;
using namespace std;

string solve(int x) {
	//+1，确保比原数大
	string s = to_string(x + 1);
	int n = s.length();
	//从高位遍历找到相邻相同的数字
	for (int i = 0; i <= n; i = 0) {
		//每次都从高位重新开始，
		for (char c = ' '; i < n && c != s[i]; i++) {
			c = s[i];
		}

		if (i == n)
			break;
		string str = s.substr(0, i + 1);
		n = str.length();
		int t = stoi(str) + 1; 
		s.replace(s.begin(), s.begin() + i + 1, to_string(t));//s位数可能增加，比如999->1009
	}
	//交替填入0和1
	bool zero = s[n - 1] - '0';
	for (int i = n; i < s.length(); i++, zero = !zero) {
		s[i] = zero ? '0' : '1';
	}

	return s;
}



int main() {
	std::ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int x;
	cin >> x;

	cout << solve(x) << endl;

	return 0;

}