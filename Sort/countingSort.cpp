#include <iostream>
#include <vector>
#include <algorithm>

constexpr int N = 1'000'00;

void countingSort(std::vector<int> &arr) {
	int n = arr.size();
	int mx = *std::max_element(arr.begin(), arr.end()) + 1;
	std::vector<int> b(n, 0), cnt(mx, 0);
	for (int i = 0; i < n; ++i) {
		cnt[arr[i]]++;
	}
	for (int i = 1; i < mx; ++i) {
		cnt[i] += cnt[i-1];
	}

	for (int i = n - 1; i >= 0; --i) {
		b[--cnt[arr[i]]] = arr[i];
	}

	std::copy(b.begin(), b.end(), arr.begin());
}

void printArray(const std::vector<int>& arr) {
	for (int num : arr) {
		std::cout << num << " ";
	}
	std::cout << std::endl;
}

int main() {
	std::vector<int> arr = { 12, 11, 13, 5, 6 };

	std::cout << "Original array: ";
	printArray(arr);

	countingSort(arr);

	std::cout << "Sorted array: ";
	printArray(arr);


	return 0;
}
