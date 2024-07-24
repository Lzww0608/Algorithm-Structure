#include <iostream>
#include <vector>
#include <queue>
#include <functional>

using namespace std;

// BFS判断二分图函数
bool isBipartiteBFS(vector<vector<int>>& graph) {
	int n = graph.size();
	vector<int> col(n, -1);

	for (int i = 0; i < n; ++i) {
		if (col[i] == -1) {
			queue<int> q;
			q.push(i);
			col[i] = 0;

			while (!q.empty()) {
				int u = q.front();
				q.pop();
				for (int v : graph[u]) {
					if (col[v] == -1) {
						col[v] = 1 - col[u];
						q.push(v);
					}
					else if (col[v] == col[u]) {
						return false;
					}
				}
			}
		}
	}

	return true;
}

// DFS判断二分图函数
bool isBipartiteDFS(vector<vector<int>>& graph) {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int n = graph.size();
	vector<int> col(n, -1);

	function<bool(int)> dfs = [&](int u) -> bool {
		for (int v : graph[u]) {
			if (col[v] == -1) {
				col[v] = 1 - col[u];
				if (!dfs(v)) return false;
			}
			else if (col[v] == col[u]) {
				return false;
			}
		}
		return true;
	};

	for (int i = 0; i < n; ++i) {
		if (col[i] == -1) {
			col[i] = 0;
			if (!dfs(i)) return false;
		}
	}

	return true;
}

int main() {
	vector<vector<int>> graph1 = {
		{1, 3},
		{0, 2},
		{1, 3},
		{0, 2}
	};

	vector<vector<int>> graph2 = {
		{1, 2},
		{0, 2},
		{0, 1, 3},
		{2}
	};

	cout << "Testing BFS method:\n";
	cout << "Graph 1 is " << (isBipartiteBFS(graph1) ? "" : "not ") << "bipartite.\n";
	cout << "Graph 2 is " << (isBipartiteBFS(graph2) ? "" : "not ") << "bipartite.\n";

	cout << "Testing DFS method:\n";
	cout << "Graph 1 is " << (isBipartiteDFS(graph1) ? "" : "not ") << "bipartite.\n";
	cout << "Graph 2 is " << (isBipartiteDFS(graph2) ? "" : "not ") << "bipartite.\n";

	return 0;
}
