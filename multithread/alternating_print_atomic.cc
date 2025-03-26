#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>


const int MAX_NUMBER = 1'000;

std::atomic<int> current_number{1};
std::atomic<bool> even_turn{false};

void print_even() {

    while (current_number <= MAX_NUMBER) {
        while (!even_turn.load(std::memory_order_acquire) && current_number <= MAX_NUMBER) {
            std::this_thread::yield;
        }

        if (current_number > MAX_NUMBER) {
            break;
        }

        int num = current_number.load(std::memory_order_relaxed);
        if (num % 2 == 0) {
            std::cout << "Even Thread: " << num << std::endl;
            current_number.fetch_add(1, std::memory_order_relaxed);
            even_turn.store(false, std::memory_order_release);
        }
    }
}

void print_odd() {
    while (current_number <= MAX_NUMBER) {
        while (even_turn.load(std::memory_order_acquire) && current_number <= MAX_NUMBER) {
            std::this_thread::yield;
        }

        if (current_number > MAX_NUMBER) {
            break;
        }

        int num = current_number.load(std::memory_order_relaxed);
        if (num % 2 == 1) {
            std::cout << "Odd Thread: " << num << std::endl;
            current_number.fetch_add(1, std::memory_order_relaxed);
            even_turn.store(true, std::memory_order_release);
        }
    }
}

// 1k Time uses: 1.7099 seconds
// 100w Time uses: 45.854 seconds
int main() {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    std::thread odd_thread(print_odd);
    std::thread even_thread(print_even);

    odd_thread.join();
    even_thread.join();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_used = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    std::cout << "Time used: " << time_used.count() << " seconds" << std::endl;

    return 0;
}