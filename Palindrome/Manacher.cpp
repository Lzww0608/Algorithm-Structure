#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

int lengthOfPalindrome(const std::string &s, int l, int r) {
	while (l >= 0 && r < s.length() && s[l] == s[r]) {
		l--;
		r++;
	}

	return r - l - 1;
}

int longestPalindrome(const std::string &s) {
	int n = s.length();
	int ans = 0;
	for (int i = 0; i < n - 1; i++) {
		ans = std::max({ans, lengthOfPalindrome(s, i, i), lengthOfPalindrome(s, i, i + 1)});
	}

	return ans;
}

std::string preProcess(const std::string& s) {
	std::string t = "^";
	for (char c : s) {
		t += "#" + std::string(1, c);
	}
	t += "#$";

	return t;
}

int Manacher(const std::string& s) {
	std::string t = preProcess(s);
	int n = t.length();
	std::vector<int> p(n, 0);
	int center = 0, right = 0;
	int maxLen = 0;

	for (int i = 1; i < n - 1; i++) {
		int mirror = 2 *center - i;

		if (i < right) {
			p[i] = std::min(right - i, p[mirror]);
		}

		while (t[i + 1 + p[i]] == t[i - 1 - p[i]]) {
			p[i]++;
		}

		if (i + p[i] > right) {
			center = i;
			right = i + p[i];
		}

		maxLen = std::max(maxLen, p[i]);
	}

	return maxLen;
}

int main() {
	std::string s;
	std::cout << "Enter a string: ";
	std::cin >> s;

	int result = longestPalindrome(s);
	std::cout << "Length of the longest palindromic substring is: " << result << std::endl;
	std::cout << "Length of the longest palindromic substring is: " << Manacher(s) << std::endl;
	return 0;
}