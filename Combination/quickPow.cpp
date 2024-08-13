#include <iostream>

using ll = int64_t;
const int MOD = 1'000'000'007;

int quickPow(ll a, ll r) {
	ll res = 1;
	while (r > 0) {
		if (r & 1) {
			res = res * a % MOD;
		}
		a = a * a % MOD;
		r >>= 1;
	}
	return res;
}

int main() {

	int base1 = 2, exp1 = 10; // 2^10 = 1024
	int base2 = 3, exp2 = 13; // 3^13 = 1594323 % 1'000'000'007
	int base3 = 5, exp3 = 20; // 5^20 = 95367431640625 % 1'000'000'007
	int base4 = 7, exp4 = 0;  // 7^0 = 1
	int base5 = 2, exp5 = 1000000000; // Large exponent test

	std::cout << "Test 1: " << quickPow(base1, exp1) << " (Expected: 1024)" << std::endl;
	std::cout << "Test 2: " << quickPow(base2, exp2) << " (Expected: 1594323)" << std::endl;
	std::cout << "Test 3: " << quickPow(base3, exp3) << " (Expected: 430973056)" << std::endl;
	std::cout << "Test 4: " << quickPow(base4, exp4) << " (Expected: 1)" << std::endl;
	std::cout << "Test 5: " << quickPow(base5, exp5) << " (Expected: 140625001)" << std::endl;

	return 0;
}
