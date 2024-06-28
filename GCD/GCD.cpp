#include <iostream>

// iteration
int GCD1(int x, int y) {
	while (y != 0) {
		int tmp = x;
		x = y;
		y = tmp % y;
	}

	return x;
}

// recursion
int GCD2(int x, int y) {
	if (y == 0) return x;
	return GCD2(y, x % y);
}

int main() {
	int x, y;
	std::cin >> x >> y;
	std::cout << "GCD1: " << GCD1(x, y) << std::endl;
	std::cout << "GCD2: " << GCD2(x, y) << std::endl;
}