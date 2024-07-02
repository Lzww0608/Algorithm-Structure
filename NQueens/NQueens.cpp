#include <iostream>
#include <string>
#include <vector>
#include <functional>

std::vector<std::vector<std::string>> NQueens(int n, int &cnt) {
	std::string s(n, '.');
	std::vector<std::vector<std::string>> ans;
	std::vector<std::string> tmp(n, s);

	std::vector<bool> col(n, false), dg(2 * n, false), udg(2 * n, false);
	std::function<void(int)> dfs = [&](int x) {
		if (x == n) {
			ans.push_back(tmp);
			cnt++;
			return;
		}

		for (int i = 0; i < n; ++i) {
			if (!col[i] && !dg[x + i] && !udg[n - 1 + x - i]) {
				col[i] = true;
				dg[x + i] = true;
				udg[n - 1 + x - i] = true;
				tmp[x][i] = 'Q';
				dfs(x + 1);
				tmp[x][i] = '.';
				col[i] = false;
				dg[x + i] = false;
				udg[n - 1 + x - i] = false;
			}
		}
	};
	dfs(0);

	return ans;
}

int main() {
	int n;
	std::cin >> n;

	if (n <= 0) {
		std::cerr << "The size of the board must be positive." << std::endl;
		return 1;
	}

	int cnt = 0;
	auto matrix = NQueens(n, cnt);
	std::cout << "Count: " << cnt << std::endl;
	for (auto &tmp : matrix) {
		for (auto &s : tmp) {
			std::cout << s << std::endl;
		}
		std::cout << std::endl;
	}

	return 0;
}
