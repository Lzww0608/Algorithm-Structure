#include <iostream>
#include <tuple>

int add(int x, int y) {
	while (y != 0) {
		std::tie(x, y) = std::make_tuple(x ^ y, (x & y) << 1);
	}

	return x;
}

// y >= 0
int multiply(int x, int y) {
	int res = 0;
	while (y != 0) {
		if ((y & 1) == 1) {
			res += x;
		}
		x <<= 1;
		y >>= 1;
	}

	return res;
}


int main() {
	std::cout << add(4, -15) << std::endl;
	std::cout << multiply(-4, 5) << std::endl;
}