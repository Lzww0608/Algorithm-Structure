#include <vector>
#include <iostream>
#include <numeric>

// The following class is the DSU template.
// The `rank` and `sz` vector is not necessary.
class Union {
private:
	std::vector<int> parent;
	std::vector<int> rank;
	std::vector<int> sz;

public:
	// The `ctor`, `find` and `merge` functions are necessary.
	Union(int n) {
		parent.resize(n);
		rank.resize(n, 0);
		sz.resize(n, 1);
		std::iota(parent.begin(), parent.end(), 0);
	}

	int find(int x) {
		if (x != parent[x]) {
			parent[x] = find(parent[x]);
		}
		return parent[x];
	}

	void merge(int x, int y) {
		int rx = find(x), ry = find(y);
		if (rx != ry) {
			if (rank[rx] < rank[ry]) {
				std::swap(rx, ry);
			}
			parent[ry] = rx;
			sz[rx] += sz[ry];
			if (rank[ry] == rank[rx]) {
				rank[rx]++;
			}
		}
	}

};

/*
There are n nodes. Some of them are connected, while some are not. 
If node 'a' is connected directly with node 'b', and node 'b' is connected directly with node 'c', then node 'a' is connected indirectly with node 'c'.
A cluster is a group of directly or indirectly connected nodes and no other nodes outside of the group.

You are given an n x n matrix isConnected where isConnected[i][j] = 1 if the ith node and the jth node are directly connected, 
and isConnected[i][j] = 0 otherwise.

Return the total number of clusters.
*/
int main() {
	std::vector<std::vector<int>> isConnected {{1,1,0}, {1,1,0}, {0,0,1}};
	int n = isConnected.size();
	Union dsu(n);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			if (isConnected[i][j] == 1) {
				dsu.merge(i, j);
			}
		}
	}

	int ans = 0;
	for (int i = 0; i < n; ++i) {
		if (i == dsu.find(i)) {
			ans++;
		}
	}
	std::cout << ans <<std::endl;
}