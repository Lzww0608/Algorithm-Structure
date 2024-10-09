/*
Given a binary matrix consisting of '0' and '1' characters, 
determine whether all the connected regions of adjacent '1' characters form rectangular shapes.
*/


#include <iostream>
#include <string>
#include <vector>

void solve() {
	int n, m;
	std::cin >> n >> m;
	
	std::vector<std::string> s(n);
	for (int i = 0; i < n; ++i) {
		std::cin >> s[i];
	}

	for (int i = 0; i < n - 1; ++i) {
		for (int j = 0; j < m - 1; ++j) {
			if (s[i][j] - '0' + s[i][j + 1] - '0' + s[i + 1][j] - '0' + s[i + 1][j + 1] - '0' == 3) {
				std::cout << "NO" << std::endl;
				return;
			}
		}
	}
	std::cout << "YES" << std::endl;
	return;
}





int main() {

	int t;
	std::cin >> t;

	while (t--) {
		solve();
	}

	return 0;
}