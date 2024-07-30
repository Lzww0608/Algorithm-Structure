#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <numeric>


void reservoirSampling(const std::vector<int> &input, std::vector<int>& reservoir, int k) {
	for (int i = 0; i < k; ++i) {
		reservoir[i] = input[i];
	}

	std::mt19937 gen(static_cast<unsigned int>(time(0)));
	for (int i = k; i < input.size(); ++i) {
		std::uniform_int_distribution<> dis(0, i);
		int j = dis(gen);
		if (j < k) {
			reservoir[j] = input[i];
		}
	}
}

int main() {
	std::vector<int> input(100000);
	std::iota(input.begin(), input.end(), 1);

	int k = 10;
	std::vector<int> reservoir(k);

	reservoirSampling(input, reservoir, k);

	std::cout << "Randomly selected numbers: ";
	int n = reservoir.size();
	for (int i = 0; i < n; ++i) {
		std::cout << reservoir[i] << " \n"[i == n - 1];
	}

	return 0;
}