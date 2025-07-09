#include <vector>
#include <iostream>
#include <algorithm>
#include <stack>
#include <cmath>
#include <string>
using namespace std;

const double EPSILON = 0.0000000001;

//二分法
double sqrt2() {
	double low = 1.4, high = 1.5;
	double mid = (low + high) / 2;
	while (high - low > EPSILON) {
		if (mid  * mid > 2) {
			high = mid;
		}
		else {
			low = mid;
		}
		mid = (low + high) / 2;
	}
	return mid;
}
//牛顿迭代法求f(x) = x ^ 2 - 2 的零点
// Xn+1 = Xn - f(Xn) / f(X'n)
double newton(double x) {
	if (abs(x * x) - 2 > EPSILON) {
		return newton(x - (x * x - 2) / (2 * x));
	}
	return x;
}

int main() {
	cout << sqrt2() << endl;
	cout << newton(1.5) << endl;
}

