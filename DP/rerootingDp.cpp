#include <iostream>
#include <vector>
#include <functional>

std::vector<int> sumOfDistancesInTree(int n, std::vector<std::vector<int>>& edges) {
	std::vector<std::vector<int>> g(n);
	for (auto&& e : edges) {
		int a = e[0], b = e[1];
		g[a].push_back(b);
		g[b].push_back(a);
	}

	std::vector<int> ans(n, 0), f(n, 0);
	std::function<void(int, int, int)> dfs1 = [&](int v, int fa, int depth) {
		ans[0] += depth;
		f[v] = 1;
		for (int u : g[v]) {
			if (u != fa) {
				dfs1(u, v, depth + 1);
				f[v] += f[u];
			}
		}
	};

	std::function<void(int, int)> dfs2 = [&](int v, int fa) {
		for (int u : g[v]) {
			if (u != fa) {
				ans[u] = ans[v] + n - f[u] - f[u];
				dfs2(u, v);
			}
		}
	};

	dfs1(0, -1, 0);
	dfs2(0, -1);

	return ans;
}

int main() {
	int n = 6;

	std::vector<std::vector<int>> edges = {
		{0, 1},
		{0, 2},
		{2, 3},
		{2, 4},
		{2, 5}
	};

	auto result = sumOfDistancesInTree(n, edges);

	std::cout << "Sum of distances in tree: ";
	for (int dist : result) {
		std::cout << dist << " ";
	}
	std::cout << std::endl;

	return 0;
}
