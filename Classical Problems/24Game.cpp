#include <iostream>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

class Solution {
	const double TARGET = 24;
	const double EPS = 1e-6;

public:
	bool judgePoint24(vector<int>& cards) {
		vector<string> expressions = { to_string(cards[0]), to_string(cards[1]), to_string(cards[2]), to_string(cards[3]) };
		return solve({ double(cards[0]), double(cards[1]), double(cards[2]), double(cards[3]) }, expressions);
	}

	bool solve(vector<double> a, vector<string>& expressions) {
		int n = a.size();
		if (n == 1) {
			if (std::abs(a[0] - TARGET) < EPS) {
				cout << "Solution: " << expressions[0] << endl;
				return true;
			}
			return false;
		}

		bool found = false;

		for (int i = 0; i < n; ++i) {
			for (int j = i + 1; j < n; ++j) {
				vector<double> tmpNumbers;
				vector<string> tmpExpressions;

				for (int k = 0; k < n; ++k) {
					if (k != i && k != j) {
						tmpNumbers.push_back(a[k]);
						tmpExpressions.push_back(expressions[k]);
					}
				}

				vector<pair<double, string>> results = cal(a[i], a[j], expressions[i], expressions[j]);

				for (auto& result : results) {
					tmpNumbers.push_back(result.first);
					tmpExpressions.push_back(result.second);
					if (solve(tmpNumbers, tmpExpressions)) {
						found = true;
					}
					tmpNumbers.pop_back();
					tmpExpressions.pop_back();
				}
			}
		}

		return found;
	}

	vector<pair<double, string>> cal(double x, double y, string sx, string sy) {
		vector<pair<double, string>> res;
		res.push_back({ x + y, "(" + sx + " + " + sy + ")" });
		res.push_back({ x - y, "(" + sx + " - " + sy + ")" });
		res.push_back({ x * y, "(" + sx + " * " + sy + ")" });
		res.push_back({ y - x, "(" + sy + " - " + sx + ")" });
		if (std::abs(y) > EPS) {
			res.push_back({ x / y, "(" + sx + " / " + sy + ")" });
		}
		if (std::abs(x) > EPS) {
			res.push_back({ y / x, "(" + sy + " / " + sx + ")" });
		}
		return res;
	}
};

int main() {
	Solution sol;
	vector<int> cards = { 8, 7, 4, 1 };
	if (!sol.judgePoint24(cards)) {
		cout << "No solution found！" << endl;
	}
	return 0;
}
