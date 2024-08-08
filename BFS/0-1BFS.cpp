#include <iostream>
#include <vector>
#include <deque>
#include <tuple>

/*
The character 'S' represents the player. The player can move up, down, left, right in grid if it is a floor (empty cell).
The character '.' represents the floor which means a free cell to walk.
The character '#' represents the wall which means an obstacle (impossible to walk there).
There is only one box 'B' and one target cell 'T' in the grid.
The box can be moved to an adjacent free cell by standing next to the box and then moving in the direction of the box. This is a push.
The player cannot walk through the box.
*/

int minPushBox(std::vector<std::vector<char>>& grid) {
	int m = grid.size(), n = grid[0].size();
	int si, sj, bi, bj;
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			if (grid[i][j] == 'B') {
				bi = i;
				bj = j;
			}
			else if (grid[i][j] == 'S') {
				si = i;
				sj = j;
			}
		}
	}

	auto f = [&](int i, int j) {
		return i * n + j;
	};

	auto check = [&](int i, int j) {
		return i >= 0 && i < m && j >= 0 && j < n && grid[i][j] != '#';
	};

	std::vector<std::vector<char>> vis(m * n, std::vector<char>(m * n, false));
	std::deque<std::tuple<int, int, int>> dq;
	dq.push_back({ f(si, sj), f(bi, bj), 0 });
	vis[f(si, sj)][f(bi, bj)] = true;
	std::vector<std::vector<int>> dirs{ {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

	while (!dq.empty()) {
		auto[s, b, d] = dq.front();
		dq.pop_front();
		si = s / n, sj = s % n;
		bi = b / n, bj = b % n;
		if (grid[bi][bj] == 'T') return d;

		for (auto &&dir : dirs) {
			int sx = si + dir[0], sy = sj + dir[1];
			if (!check(sx, sy)) continue;

			if (sx == bi && sy == bj) {
				int bx = bi + dir[0], by = bj + dir[1];
				if (check(bx, by) && !vis[f(sx, sy)][f(bx, by)]) {
					vis[f(sx, sy)][f(bx, by)] = true;
					dq.push_back({ f(sx, sy), f(bx, by), d + 1 });
				}
			}
			else if (!vis[f(sx, sy)][f(bi, bj)]) {
				vis[f(sx, sy)][f(bi, bj)] = true;
				dq.push_front({ f(sx, sy), f(bi, bj), d });
			}
		}
	}

	return -1;
}

int main() {
	std::vector<std::vector<char>> grid = {
		{'#', '#', '#', '#', '#', '#'},
		{'#', 'T', '#', '#', '#', '#'},
		{'#', '.', '.', 'B', '.', '#'},
		{'#', '.', '#', '#', '.', '#'},
		{'#', '.', '.', '.', 'S', '#'},
		{'#', '#', '#', '#', '#', '#'}
	};

	int result = minPushBox(grid);
	std::cout << "The minimum number of pushes to move the box to the target is: " << result << std::endl;

	return 0;
}
