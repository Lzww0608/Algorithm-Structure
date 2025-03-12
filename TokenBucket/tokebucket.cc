#include <chrono>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <thread>

class TokenBucket {
private:
    double capacity_;
    double refill_rate_;
    double tokens_;
    std::chrono::time_point<std::chrono::system_clock> last_refill_time_;
    std::mutex mutex_;

    void refill();

public:
    TokenBucket(double capacity, double refill_rate);
    bool try_consume(int tokens);
    double get_tokens();
};

TokenBucket::TokenBucket(double capacity, double refill_rate):
    capacity_(capacity), refill_rate_(refill_rate), tokens_(capacity) {
    last_refill_time_ = std::chrono::system_clock::now();
}

void TokenBucket::refill() {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = now - last_refill_time_;
    double tokens_to_add = elapsed_seconds.count() * refill_rate_;

    tokens_ = std::min(capacity_, tokens_ + tokens_to_add);
    last_refill_time_ = now;
}

bool TokenBucket::try_consume(int tokens_needed) {
    std::lock_guard<std::mutex> lock(mutex_);
    refill();
    if (tokens_ >= tokens_needed) {
        tokens_ -= tokens_needed;
        return true;
    }
    return false;
}

double TokenBucket::get_tokens() {
    std::lock_guard<std::mutex> lock(mutex_);
    return tokens_;
}

int main() {
    TokenBucket token_bucket(10, 1);
    for (int i = 0; i < 15; i++) {
        int tokens_to_consume = (i % 3 == 0) ? 3 : 1;
        if (token_bucket.try_consume(tokens_to_consume)) {
            std::cout << "Consumed " << tokens_to_consume << " tokens" << std::endl;
        } else {
            std::cout << "Failed to consume " << tokens_to_consume << " tokens" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}