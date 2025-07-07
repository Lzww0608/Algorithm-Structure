#include <random>
#include <cstdio>
#include <iostream>
#include <iomanip>

constexpr double EPSILON = 1e-9;

long double estimatePi() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<long double> dis(0.0L, 1.0L);

    uint64_t inside_points = 0;
    uint64_t total_points = 1'000'000'000;

    for (uint64_t i = 0; i < total_points; ++i) {
        long double x = dis(gen);
        long double y = dis(gen);

        if (x * x + y * y + EPSILON <= 1.0L) {
            ++inside_points;
        }
    }

    return 4.0L * inside_points / total_points;
}

int main() {
    std::cout << std::fixed << std::setprecision(10) << estimatePi() << std::endl;
    return 0;
}