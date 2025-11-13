/*
@Author: Lzww
@LastEditTime: 2025-9-26 10:00:49
@Description: 骑士周游问题
https://www.luogu.com.cn/problem/solution/UVA10255
启发式搜索，每次从可以选择的点中计算评估函数f(a, b), a为该点可以移动的数目，b为该点离中心点的距离，
按照a从小到大，b从大到小排序，选择f(a, b)最小的点，这样每次先选择难以到达以及更边缘的点。
@Language: C++17
*/

#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>
#include <cmath>
#include <queue>
#include <utility>
#include <tuple>
#include <ranges>
#include <functional>
#include <chrono>
#include <vector>
#include <array>
#include <random>
#include <stack>
#include <bitset>
#include <deque>
#include <ios>
#include <list>
#include <cstdint>
#include <limits>
#include <limits.h>
#include <cstdio>
#include <shared_mutex>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <thread>
#include <random>

#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>
#include <cmath>
#include <queue>
#include <utility>
#include <tuple>
#include <ranges>
#include <functional>
#include <chrono>
#include <vector>
#include <array>
#include <random>
#include <stack>
#include <bitset>
#include <deque>
#include <ios>
#include <list>
#include <cstdint>
#include <limits>
#include <limits.h>
#include <cstdio>
#include <shared_mutex>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <thread>
#include <random>

using i64 = std::int64_t;
using pii = std::pair<i64, i64>;
using tii = std::tuple<i64, i64, i64>;

constexpr i64 N = 51;
constexpr i64 M = 8;
constexpr i64 MOD = 1e8-3;

std::array<std::array<int, N>, N> g;
std::array<int, M> dx {1, 1, 2, 2, -1, -1, -2, -2};
std::array<int, M> dy {2, -2, 1, -1, 2, -2, 1, -1};

int n, a, b;

inline bool check(int x, int y) {
	if (x < 0 || x >= n || y < 0 || y >= n || g[x][y] != 0) {
		return false;
	}

	return true;
}

pii prune(int i, int j) {
	int cnt = 0, dis = int((i - double(n) / 2) * (i - double(n) / 2) + (j - double(n) / 2) * (j - double(n) / 2));
	for (int k = 0; k < M; k++) {
		int x = i + dx[k], y = j + dy[k];
		if (check(x, y)) {
			cnt++;
		}
	}

	return {cnt, dis};
}

bool solve(int i, int j, int cnt) {
	g[i][j] = cnt;
	if (cnt == n * n) {
		return true;
	}

	std::vector<tii> p;
	std::vector<int> id;
	for (int k = 0; k < M; k++) {
		int x = i + dx[k], y = j + dy[k];
		if (check(x, y)) {
			auto tmp = prune(x, y);
			p.emplace_back(tmp.first, tmp.second, k);
			id.push_back(id.size());
		}
	}

	std::ranges::sort(id, [&] (const auto& v, const auto& u) {
		return std::get<0>(p[v]) < std::get<0>(p[u]) || std::get<0>(p[v]) == std::get<0>(p[u]) && std::get<1>(p[v]) > std::get<1>(p[u]);
	});

	for (const auto& v : id) {
		int k = std::get<2>(p[v]);
		int x = i + dx[k], y = j + dy[k];
		if (solve(x, y, cnt + 1)) {
			return true;
		}
	}

	g[i][j] = 0;
	return false;
}


int main() {
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while (std::cin >> n >> a >> b) {
		g = {};
		a--;
		b--;
		if (n % 2 == 1 || n <= 5 || !solve(a, b, 1)) {
			std::cout << "No Circuit Tour." << "\n";
		} else {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					std::cout << g[i][j] << " ";
				}
				std::cout << "\n";
			}
		}
		std::cout << "\n";
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_used = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	std::cout << "Time used: " << time_used.count() << " seconds" << std::endl;
	return 0;
}
