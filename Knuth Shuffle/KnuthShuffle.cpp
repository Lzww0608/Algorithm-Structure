#include <iostream>
#include <random>
#include <vector>
#include <numeric>
#include <ctime>

constexpr int N = 52;

int main() {
	std::vector<int> poker(N);
	std::iota(poker.begin(), poker.end(), 1); // 初始化为1到52

	std::mt19937 gen(static_cast<unsigned int> (time(0)));

	for (int i = 0; i < N; ++i) {
		std::uniform_int_distribution<> dis(0, N - 1);
		int j = dis(gen);
		std::swap(poker[i], poker[j]);
	}

	for (int i = 0; i < N; ++i) {
		switch ((poker[i] - 1) / 13) {
		case 0:
			std::cout << "红桃"; break;
		case 1:
			std::cout << "方片"; break;
		case 2:
			std::cout << "黑桃"; break;
		case 3:
			std::cout << "梅花"; break;
		}

		switch ((poker[i] - 1) % 13) {
		case 0:
			std::cout << "A"; break;
		case 10:
			std::cout << "J"; break;
		case 11:
			std::cout << "Q"; break;
		case 12:
			std::cout << "K"; break;
		default:
			std::cout << (poker[i] - 1) % 13 + 1; break;
		}

		if ((i + 1) % 13 == 0) std::cout << "\n";
		else std::cout << " ";
	}

	return 0;
}
