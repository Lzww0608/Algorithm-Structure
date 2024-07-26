#include <iostream>
#include <vector>
#include <limits>

const int INF = std::numeric_limits<int>::max();

std::vector<std::vector<int>> Floyd(std::vector<std::vector<int>>& g) {
	int n = g.size();

	for (int k = 0; k < n; ++k) {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				if (g[i][k] != INF && g[k][j] != INF && g[i][j] > g[i][k] + g[k][j]) {
					g[i][j] = g[i][k] + g[k][j];
				}
			}
		}
	}

	return g;
}

void printGraph(const std::vector<std::vector<int>>& g) {
	int n = g.size();
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			if (g[i][j] == INF) {
				std::cout << "INF ";
			}
			else {
				std::cout << g[i][j] << " ";
			}
		}
		std::cout << std::endl;
	}
}

int main() {

	std::vector<std::vector<int>> graph = {
		{0, 3, INF, 5},
		{2, 0, INF, 4},
		{INF, 1, 0, INF},
		{INF, INF, 2, 0}
	};

	std::cout << "Original graph:" << std::endl;
	printGraph(graph);

	std::vector<std::vector<int>> shortestPaths = Floyd(graph);

	std::cout << "Graph after applying Floyd-Warshall algorithm:" << std::endl;
	printGraph(shortestPaths);

	return 0;
}
