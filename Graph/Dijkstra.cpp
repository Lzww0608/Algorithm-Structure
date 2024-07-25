#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>
#include <queue>


std::vector<int> dijkstra(std::vector<std::vector<std::pair<int, int>>> g, int start) {
	int n = g.size();
	std::vector<int> dist(n, 0x3f3f3f3f); 

	dist[start] = 0;

	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
	pq.push({ 0, start });

	while (!pq.empty()) {
		auto tmp = pq.top();
		pq.pop();
		int d = tmp.first;  // distance
		int u = tmp.second; // target node

		if (dist[u] < d) continue;

		for (auto e : g[u]) {
			int v = e.first;
			int weight = e.second;
			if (dist[u] + weight < dist[v]) {
				dist[v] = dist[u] + weight;
				pq.push({ dist[v], v });
			}
		}
	}

	return dist;
}

int main() {
	int n = 5; 
	std::vector<std::vector<std::pair<int, int>>> graph(n);

	// 添加边 (u, v, weight)
	graph[0].push_back({ 1, 10 });
	graph[0].push_back({ 4, 5 });
	graph[1].push_back({ 2, 1 });
	graph[1].push_back({ 4, 2 });
	graph[2].push_back({ 3, 4 });
	graph[3].push_back({ 2, 6 });
	graph[3].push_back({ 0, 7 });
	graph[4].push_back({ 1, 3 });
	graph[4].push_back({ 2, 9 });
	graph[4].push_back({ 3, 2 });

	int start = 0; // 起点

	std::vector<int> distances = dijkstra(graph, start);

	std::cout << "从起点 " << start << " 到其他节点的最短距离:" << std::endl;
	for (int i = 0; i < distances.size(); ++i) {
		std::cout << "节点 " << i << ": " << distances[i] << std::endl;
	}

	return 0;
}
