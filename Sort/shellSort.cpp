#include <iostream>
#include <vector>

void shellSort(std::vector<int>& arr) {
	int n = arr.size();
	
	for (int gap = n / 2; gap >= 1; gap /= 2) {
		for (int i = gap; i < n; ++i) {
			int tmp = arr[i];
			int j;
			for (j = i; j >= gap && arr[j - gap] > tmp; j -= gap) {
				arr[j] = arr[j-gap];
			}
			arr[j] = tmp;
		}
	}

	return;
}

int main() {
	std::vector<int> arr = { 12, 34, 54, 2, 3 };

	shellSort(arr);
	int n = arr.size();
	for (int i = 0; i < n; ++i) {
		std::cout << arr[i] << " \n"[i == n - 1];
	}

}