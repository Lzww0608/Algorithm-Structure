#include <iostream>
#include <stack>
#include <vector>
#include <random>
#include <utility>


std::pair<int, int> partition(std::vector<int> &arr, int l, int r) {
	int pivot = arr[l + rand() % (r - l + 1)];
	int i = l, j = l, k = r + 1;

	while (i < k) {
		if (arr[i] > pivot) {
			std::swap(arr[i], arr[--k]);
		}
		else if (arr[i] < pivot) {
			std::swap(arr[i++], arr[j++]);
		}
		else {
			i++;
		}
	}

	return {j, k-1};
}

void quickSortIterative(std::vector<int> &arr, int l, int r) {
	std::stack<int> st;

	st.push(l);
	st.push(r);

	while (!st.empty()) {
		r = st.top();
		st.pop();
		l = st.top();
		st.pop();

		auto [p, q] = partition(arr, l, r);
		if (p - 1 > l) {
			st.push(l);
			st.push(p-1);
		}

		if (q + 1 < r) {
			st.push(q + 1);
			st.push(r);
		}
	}
}

// 打印数组
void printArray(std::vector<int>& arr, int size) {
	for (int i = 0; i < size; i++) {
		std::cout << arr[i] << " \n"[i==size-1];
	}
}

int main() {
	std::vector<int> arr = { 10, 7, 8, 9, 1, 5 };
	int n = arr.size();
	quickSortIterative(arr, 0, n - 1);
	std::cout << "排序后的数组: ";
	printArray(arr, n);
	return 0;
}