#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <gtest/gtest.h>


template <typename T>
class ThreadSafeQueue {
private:
    std::queue<T> q;
    std::mutex mtx;
    std::condition_variable cv;

public:
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(item);
        cv.notify_all();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] {return !q.empty();});

        T item = q.front();
        q.pop();
        return item;
    }

    bool empty() const {
        return q.empty();
    }
    
};


void producer(ThreadSafeQueue<int>& queue) {
    for (int i = 0; i < 10; i++) {
        queue.push(i);
        std::cout << "Produced: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


void consumer(ThreadSafeQueue<int>& queue) {
    for (int i = 0; i < 10; i++) {
        int item = queue.pop();
        std::cout << "Consumed: " << item << std::endl;
    }
}


TEST(ThreadSafeQueueTest, Basic) {
    ThreadSafeQueue<int> queue;
    std::thread producerThread(producer, std::ref(queue));
    std::thread consumerThread(consumer, std::ref(queue));

    producerThread.join();
    consumerThread.join();

    EXPECT_TRUE(queue.empty());
}

