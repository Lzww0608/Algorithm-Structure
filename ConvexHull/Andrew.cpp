#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iostream>


std::vector<std::vector<int>> Andrew(std::vector<std::vector<int>>& points) {
	auto substract = [](const std::vector<int>& a, const std::vector<int>& b) {
		return std::vector<int> {a[0] - b[0], a[1] - b[1]};
	};

	auto cross = [](const std::vector<int>& a, const std::vector<int>& b) {
		return a[0] * b[1] - a[1] * b[0];
	};

	auto getArea = [&](const std::vector<int>& a, const std::vector<int>& b, const std::vector<int>& c) {
		return cross(substract(b, a), substract(c, a));
	};

	std::sort(points.begin(), points.end(), [] (std::vector<int>& a, std::vector<int>& b) {
		return a[0] < b[0] || a[0] == b[0] && a[1] < b[1];
	});

	int n = points.size();
	if (n <= 1) return points;

	std::vector<int> st;
	st.reserve(n + 5);
	for (int i = 0; i < n; ++i) {
		while (st.size() >= 2 && getArea(points[st[st.size()-2]], points[st[st.size()-1]], points[i]) < 0 ) {
			st.pop_back();
		}
		st.push_back(i);
	}

	int sz = st.size();
	for (int i = n - 2; i >= 0; i--) {
		while (st.size() > sz && getArea(points[st[st.size() - 2]], points[st[st.size() - 1]], points[i]) < 0) {
			st.pop_back();
		}
		st.push_back(i);
	}

	st.pop_back();

	std::vector<std::vector<int>> ans;
	std::unordered_map<int, bool> vis;

	for (int i : st) {
		if (!vis[i]) {
			ans.push_back(points[i]);
			vis[i] = true;
		}
	}

	return ans;
}


int main() {
	std::vector<std::vector<int>> points = {
		{1, 1}, {2, 2}, {2, 0}, {2, 4}, {3, 3}, {4, 2}
	};

	std::vector<std::vector<int>> convexHull = Andrew(points);

	std::cout << "The points on the convex hull are:" << std::endl;
	for (const auto& point : convexHull) {
		std::cout << "(" << point[0] << ", " << point[1] << ")" << std::endl;
	}

	return 0;
}