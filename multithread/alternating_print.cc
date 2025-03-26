#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
std::mutex mtx;
std::condition_variable cv;
int current_number = 1;
bool even_turn = false;

const int MAX_NUMBER = 1'000;

void print_even() {
    while (current_number <= MAX_NUMBER) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] {return even_turn || current_number > MAX_NUMBER;});

        if (current_number > MAX_NUMBER) {
            break;
        }

        if (current_number % 2 == 0) {
            std::cout << "Even Thread: " << current_number << std::endl;
            current_number++;
            even_turn = false;
        }
        
        cv.notify_one();
    }
}

void print_odd() {
    while (current_number <= MAX_NUMBER) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] {return !even_turn || current_number > MAX_NUMBER;});

        if (current_number > MAX_NUMBER) {
            break;
        }

        if (current_number % 2 == 1) {
            std::cout << "Odd Thread: " << current_number << std::endl;
            current_number++;
            even_turn = true;
        }

        cv.notify_one();
    }
}

// 1w Time uses: 0.0750917 seconds
// 100w Time uses: 33.8143 seconds
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