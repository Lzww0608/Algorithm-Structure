/*
You can perform actions of two types for two chosen variables with indices i and j, where i<j:
1. Perform assignment ai=ai+aj
2. Perform assignment aj=aj−ai
Your task is to change the values of all internal variables from a1,a2,…,an to −a1,−a2,…,−an.
Output the the form  as "type i j", where "type" is equal to "1" if the strategy 
needs to perform an assignment of the first type and "2" if the strategy needs to perform an assignment of the second type. 
Note that i < j should hold.
The number of a is an even number;
*/

#include <iostream>
#include <vector>


void solve(std::vector<int> &a) {
	int n = a.size();
	std::cout << "The number of the operation is: " << n * 3 << std::endl;
	for (int i = 0, j = n - 1; i < j; ++i, --j) {
		for (int k = 0; k < 3; ++k) {
			std::cout << "Operation 1: " << 1 << " operate " << i << " and " << j << std::endl;
			std::cout << "Operation 2: " << 2 << " operate " << i << " and " << j << std::endl;
		}
	}
}



int main() {
	int n;
	std::cin >> n;
	std::vector<int> a(n, 0);
	for (int i = 0; i < n; ++i) {
		std::cin >> a[i];
	}

	solve(a);
}