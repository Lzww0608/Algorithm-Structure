#include <iostream>
#include <string>
#include <cassert>

std::string add(std::string s) {
	int n = s.length();
	assert(s.length() != 0);

	if (s[0] == '-') {
		if (s == "-1") return "0";
		for (int i = n - 1; i >= 1; --i) {
			if (s[i] != '0') {
				s[i]--;
				break;
			}
			s[i] = '9';
		}
		int k = 1;
		while (s[k] == '0') k++;
		return "-" + s.substr(k);
	}

	for (int i = n - 1; i >= 0; --i) {
		if (s[i] != '9') {
			s[i]++;
			return s;
		}
		s[i] = '0';
	}

	return "1" + s;
}

std::string solve(std::string x) {
	std::string s = add(x);
	int n = s.length();

	for (int i = 0; i < n; ++i) {
		for (char c = ' '; i < n && c != s[i]; ++i) {
			c = s[i];
		}

		if (i == n) break;
		std::string str = s.substr(0, i + 1);
		std::string t = add(str);
		n = str.length();

		s.replace(s.begin(), s.begin() + i + 1, t);
	}

	bool zero = s[n-1] - '0';
	for (int i = n; i < s.length(); ++i, zero = !zero) {
		s[i] = zero ? '0' : '1';
	}

	return s;
}

int main() {
	std::string x;
	while (std::cin >> x) {
		std::cout << solve(x) << "\n";
	}

	return 0;
}