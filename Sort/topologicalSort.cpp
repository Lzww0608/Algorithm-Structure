#include <vector>
#include <queue>
#include <iostream>

std::vector<int> topological(std::vector<std::vector<int>>& Adj, std::vector<int>& In, int n) {
	std::queue<int> q;
	for (int i = 0; i < n; ++i) {
		if (In[i] == 0) {
			q.push(i);
		}
	}

	std::vector<int> ans;
	while (!q.empty()) {
		int u = q.front();
		q.pop();
		ans.push_back(u);
		for (int v : Adj[u]) {
			if (--In[v] == 0) {
				q.push(v);
			}
		}
	}

	if (ans.size() != n) return {}; 

	return ans;
}

int main() {
	int n = 6; 
	std::vector<std::vector<int>> Adj = {
		{2, 3},
		{3, 4},
		{4},
		{5},
		{5},
		{}
	};
	std::vector<int> In = { 0, 0, 1, 2, 2, 2 };

	std::vector<int> result = topological(Adj, In, n);
	if (result.empty()) {
		std::cout << "Cycle detected. No valid topological ordering exists." << std::endl;
	}
	else {
		std::cout << "Topological ordering: ";
		for (int node : result) {
			std::cout << node << " ";
		}
		std::cout << std::endl;
	}

	return 0;
}
