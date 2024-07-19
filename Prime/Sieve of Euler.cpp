#include <iostream>
#include <vector>
using ll = int64_t;

// 1: prime
std::vector<int> linearSieve(int n) {
	std::vector<int> isPrimes(n + 1, 1);
	std::vector<int> primes;
	isPrimes[0] = isPrimes[1] = 0;
	for (int i = 2; i <= n; ++i) {
		if (isPrimes[i] == 1) {
			primes.push_back(i);
		}
		for (int j = 0; j < primes.size() && i * primes[j] <= n; ++j) {
			isPrimes[i * primes[j]] = 0;
			if (i % primes[j] == 0) {
				break;
			}
		}
	}
	return isPrimes;
}

int main() {
	int n;
	std::cout << "Enter the value of n: ";
	std::cin >> n;

	std::vector<int> primes = linearSieve(n);

	std::cout << "Prime numbers up to " << n << " are: ";
	for (int i = 2; i <= n; ++i) {
		if (primes[i] == 1) {
			std::cout << i << " ";
		}
	}
	std::cout << std::endl;

	return 0;
}
