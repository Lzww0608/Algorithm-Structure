#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

/*
Find all the critical and pseudo-critical edges in the given graph's minimum spanning tree (MST).
*/

int find(std::vector<int>& fa, int x) {
	if (x != fa[x]) {
		fa[x] = find(fa, fa[x]);
	}

	return fa[x];
}

int MST(std::vector<std::vector<int>>& edges, int n, int f = -1) {
	std::vector<int> fa(n, 0);
	std::iota(fa.begin(), fa.end(), 0);
	int res = 0;

	if (f != -1) {
		res += edges[f][2];
		int u = find(fa, edges[f][0]), v = find(fa, edges[f][1]);
		fa[u] = v;
	}

	std::vector<int> q(edges.size());
	std::iota(q.begin(), q.end(), 0);

	std::sort(q.begin(), q.end(), [&] (const auto& a, const auto& b) -> bool {
		return edges[a][2] < edges[b][2];
	});

	for (int x : q) {
		int u = find(fa, edges[x][0]), v = find(fa, edges[x][1]);
		if (u != v) {
			res += edges[x][2];
		}
		fa[u] = v;
	}

	return res;
}

std::vector<std::vector<int>> findCriticalAndPseudoCriticalEdges(int n, std::vector<std::vector<int>> &edges) {
	std::vector<int> keys, fkeys;
	int mst = MST(edges, n);
	for (size_t i = 0; i < edges.size(); ++i) {
		edges[i][2]++;
		bool ok = false;
		if (MST(edges, n) > mst) {
			ok = true;
			keys.push_back(i);
		}
		edges[i][2]--;
		if (!ok && MST(edges, n, i) == mst) {
			fkeys.push_back(i);
		}
	}

	return {keys, fkeys};
}

int main() {

	std::vector<std::vector<int>> edges = {
		{0, 1, 1},
		{1, 2, 1},
		{2, 3, 2},
		{0, 3, 2},
		{0, 4, 3},
		{3, 4, 3}, 
		{1, 4, 6}
	};
	int n = 5;  

	std::vector<std::vector<int>> result = findCriticalAndPseudoCriticalEdges(n, edges);

	std::cout << "Critical Edges: ";
	for (int i : result[0]) {
		std::cout << i << " ";
	}
	std::cout << std::endl;

	std::cout << "Pseudo-Critical Edges: ";
	for (int i : result[1]) {
		std::cout << i << " ";
	}
	std::cout << std::endl;

	return 0;
}