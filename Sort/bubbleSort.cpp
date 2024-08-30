#include <vector>
#include <iostream>


void bubbleSort(std::vector<int>& arr) {
	int n = arr.size();

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				std::swap(arr[j], arr[j+1]);
			}
		}
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

	bubbleSort(arr);

	std::cout << "Sorted array: ";
	printArray(arr);


	return 0;
}

