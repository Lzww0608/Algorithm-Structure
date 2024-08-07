#include <iostream>
#include <vector>

// Gosper's hack function to generate all k-sized subsets
void gosperHack(int k, int limit) {
	int subset = (1 << k) - 1;
	while (subset < limit) {

		std::vector<int> elements;
		for (int i = 0; i < 32; ++i) {
			if (subset & (1 << i)) {
				elements.push_back(i);
			}
		}
		for (int i = 0; i < elements.size(); ++i) {
			std::cout << elements[i] << (i < elements.size() - 1 ? ", " : "\n");
		}

		int lb = subset & -subset;
		int x = subset + lb;
		subset = ((subset ^ x) / lb >> 2) | x;
	}
}

int main() {
	int n = 5; // Total number of elements
	int k = 3; // Size of subsets

	int limit = 1 << n;

	std::cout << "Generating all subsets of size " << k << " from set {0, 1, 2, ..., " << n - 1 << "}:\n";
	gosperHack(k, limit);

	return 0;
}
