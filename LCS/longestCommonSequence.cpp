#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

int longestCommonSequence(std::string a, std::string b) {
	int m = a.length(), n = b.length();

	std::vector<std::vector<int>> f(m + 1, std::vector<int> (n + 1, 0));

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			if (a[i] == b[j]) {
				f[i+1][j+1] = f[i][j] + 1;
			} else {
				f[i+1][j+1] = std::max({f[i][j], f[i+1][j], f[i][j+1]});
			}
		}
	}

	return f[m][n];
}


void testLCS() {
	struct TestCase {
		std::string a;
		std::string b;
		int expected;
	};

	std::vector<TestCase> testCases = {
		{"ABCBDAB", "BDCAB", 4},
		{"AGGTAB", "GXTXAYB", 4},
		{"ABC", "DEF", 0},
		{"ABCDGH", "AEDFHR", 3},
		{"", "", 0},
		{"A", "A", 1},
		{"ABCDE", "ABCDE", 5},
		{"AAAA", "AA", 2}
	};

	for (const auto& test : testCases) {
		int result = longestCommonSequence(test.a, test.b);
		std::cout << "LCS of \"" << test.a << "\" and \"" << test.b << "\" is: " << result << std::endl;
		assert(result == test.expected);
	}
}

int main() {
	testLCS();
	std::cout << "All test cases passed!" << std::endl;
	return 0;
}