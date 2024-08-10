#include <vector>
#include <tuple>
#include <queue>
#include <iostream>

/*
You have an undirected, connected graph of n nodes labeled from 0 to n - 1. 
You are given an array graph where graph[i] is a list of all the nodes connected with node i by an edge.

Return the length of the shortest path that visits every node. 
You may start and stop at any node, you may revisit nodes multiple times, and you may reuse edges.
*/

int shortestPathLength(std::vector<std::vector<int>>& graph) {
	int n = graph.size();
	std::vector<std::vector<int>> vis(n, std::vector<int>(1 << n, 0));
	std::queue<std::tuple<int, int, int>> q;
	for (int i = 0; i < n; ++i) {
		q.emplace(i, 1 << i, 0);
		vis[i][1 << i] = 1;
	}
	int ans = 0;
	while (!q.empty()) {
		auto[u, mask, dis] = q.front();
		q.pop();
		if (mask == (1 << n) - 1) {
			return dis;
		}
		for (auto v : graph[u]) {
			int mask_v = mask | (1 << v);
			if (!vis[v][mask_v]) {
				vis[v][mask_v] = 1;
				q.push({ v, mask_v, dis + 1 });
			}
		}
	}
	return -1;
}


int main() {
	// test1
	std::vector<std::vector<int>> graph1 = {
		{1, 2, 3},   // 0 -> 1, 2, 3
		{0},         // 1 -> 0
		{0},         // 2 -> 0
		{0}          // 3 -> 0
	};
	std::cout << "Shortest Path Length for graph1: " << shortestPathLength(graph1) << std::endl;

	// test2
	std::vector<std::vector<int>> graph2 = {
		{1},         // 0 -> 1
		{0, 2, 3},   // 1 -> 0, 2, 3
		{1},         // 2 -> 1
		{1}          // 3 -> 1
	};
	std::cout << "Shortest Path Length for graph2: " << shortestPathLength(graph2) << std::endl;

	// test3
	std::vector<std::vector<int>> graph3 = {
		{1, 4},      // 0 -> 1, 4
		{0, 2, 3},   // 1 -> 0, 2, 3
		{1},         // 2 -> 1
		{1},         // 3 -> 1
		{0}          // 4 -> 0
	};
	std::cout << "Shortest Path Length for graph3: " << shortestPathLength(graph3) << std::endl;

	return 0;
}