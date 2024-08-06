#include <iostream>
#include <utility>

// n > 6, 
// a + b = n, gcd(a, b) = 1, a > 1, b > 1 
std::pair<int, int> decompose(int n) {
	if (n % 2 == 1) {
		return { n / 2, n - n / 2 };
	}

	if ((n / 2) % 2 == 0) {
		return { n / 2 - 1, n / 2 + 1 };
	}

	return { n / 2 - 2, n / 2 + 2 };
}


int main() {
	int test_cases[] = { 15, 28, 30, 42, 50 };
	for (int n : test_cases) {
		std::pair<int, int> result = decompose(n);
		std::cout << "Decomposing " << n << " into " << result.first << " and " << result.second << std::endl;
	}

	return 0;
}


