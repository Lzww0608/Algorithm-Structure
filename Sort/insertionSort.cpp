#include <vector>
#include <iostream>


void insertionSort(std::vector<int>& arr) {
	int n = arr.size();

	for (int i = 1; i < n; ++i) {
		int tmp = arr[i];
		int j = 0;
		for (j = i - 1; j >= 0 && arr[j] > tmp; --j) {
			arr[j+1] = arr[j];
		}

		arr[j+1] = tmp;
	}
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

	insertionSort(arr);

	std::cout << "Sorted array: ";
	printArray(arr);

	return 0;
}

