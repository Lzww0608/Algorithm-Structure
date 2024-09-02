#include <vector>
#include <iostream>


void selectionSort(std::vector<int>& arr) {
	int n = arr.size();

	for (int i = 0; i < n; ++i) {
		int mn = i;
		for (int j = i + 1; j < n; ++j) {
			if (arr[j] < arr[mn]) {
				mn = j;
			}
		}

		std::swap(arr[mn], arr[i]);
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

	selectionSort(arr);

	std::cout << "Sorted array: ";
	printArray(arr);


	return 0;
}

