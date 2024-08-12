#include <iostream>
#include <vector>

using ll = int64_t;
const int MOD = 1'000'000'007;

/*
费马小定理：
a ^ (-1) = a ^ (p - 2)
*/
ll modInverse(ll x) {
	ll res = 1, exp = MOD - 2;
	while (exp > 0) {
		if (exp % 2 == 1) {
			res = res * x % MOD;
		}
		exp >>= 1;
		x = x * x % MOD;
	}

	return res;
}

int combination(int n, int k) {
	if (k > n) return 0;
	ll numerator = 1, denominator = 1;

	for (int i = 0; i < k; ++i) {
		numerator = numerator * (n - i) % MOD;
	}
	for (int i = 1; i <= k; ++i) {
		denominator = denominator * i % MOD;
	}

	return numerator * modInverse(denominator) % MOD;
}

int main() {

	std::vector<std::pair<int, int>> test_cases = {
		{5, 2},
		{10, 5},
		{100, 50},
		{10, 0},
		{10, 10},
		{1, 1},
		{0, 0},
		{7, 3},
		{7, 8},  
	};

	for (const auto&[n, k] : test_cases) {
		std::cout << "C(" << n << ", " << k << ") = " << combination(n, k) << std::endl;
	}

	return 0;
}
