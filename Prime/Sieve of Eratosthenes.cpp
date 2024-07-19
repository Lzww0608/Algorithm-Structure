#include <iostream>
#include <vector>
using ll = int64_t;

// 1: not prime 
// 0: is prime
std::vector<int> prime(int n) {
	std::vector<int> isPrimes(n + 1, 0);

	for (int i = 2; i <= n; ++i) {
		if (isPrimes[i] == 1) continue;
		for (ll j = (ll)i * i; j <= n; j += i) {
			isPrimes[j] = 1;
		}
	}

	return isPrimes;
}

int main() {
	int n;
	std::cout << "Enter the value of n: ";
	std::cin >> n;

	std::vector<int> primes = prime(n);

	std::cout << "Prime numbers up to " << n << " are: ";
	for (int i = 2; i <= n; ++i) {
		if (primes[i] == 0) {
			std::cout << i << " \n"[i == n];
		}
	}

	return 0;
}
