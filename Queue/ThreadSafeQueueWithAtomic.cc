#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <memory>
#include <gtest/gtest.h>

template<typename T>
class ThreadSafeQueue {
private:
    struct Node {
        std::shared_ptr<T> data;
        std::atomic<Node*> next;
        
        Node() : next(nullptr) {}
        
        explicit Node(const T& value) : data(std::make_shared<T>(value)), next(nullptr) {}
    };
    
    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    std::atomic<size_t> size;
    
public:
    ThreadSafeQueue() : size(0) {
        Node* dummy = new Node();
        head.store(dummy);
        tail.store(dummy);
    }
    
    ~ThreadSafeQueue() {
        while (pop_if_not_empty() != nullptr) {}
        
        // 删除哑节点
        delete head.load();
    }
    
    // 禁止拷贝和赋值
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
    
    void push(const T& value) {
        Node* new_node = new Node(value);
        Node* old_tail = tail.load(std::memory_order_relaxed);
        
        while (true) {
            // 尝试将新节点链接到尾节点
            Node* expected_null = nullptr;
            if (old_tail->next.compare_exchange_weak(
                    expected_null, new_node, 
                    std::memory_order_release, 
                    std::memory_order_relaxed)) {
                // 成功链接，更新尾指针
                tail.store(new_node, std::memory_order_release);
                size.fetch_add(1, std::memory_order_relaxed);
                return;
            }
            
            // 其他线程已经将节点链接到尾部，协助更新尾指针
            tail.compare_exchange_weak(
                old_tail, old_tail->next.load(std::memory_order_relaxed),
                std::memory_order_release,
                std::memory_order_relaxed);
                
            // 重新获取尾节点
            old_tail = tail.load(std::memory_order_relaxed);
        }
    }
    
    std::shared_ptr<T> pop() {
        std::shared_ptr<T> result;
        
        while (true) {
            result = pop_if_not_empty();
            if (result) {
                return result;
            }
            
            // 队列为空，继续尝试
            std::this_thread::yield();
        }
    }
    
    std::shared_ptr<T> try_pop() {
        return pop_if_not_empty();
    }
    
    bool empty() const {
        return size.load(std::memory_order_relaxed) == 0;
    }
    
    size_t get_size() const {
        return size.load(std::memory_order_relaxed);
    }
    
private:
    std::shared_ptr<T> pop_if_not_empty() {
        Node* old_head = head.load(std::memory_order_relaxed);
        
        while (true) {
            Node* first = old_head->next.load(std::memory_order_acquire);
            if (!first) {
                // 队列为空
                return nullptr;
            }
            
            // 尝试更新头节点
            if (head.compare_exchange_weak(
                    old_head, first,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                // 成功取出元素
                std::shared_ptr<T> result = first->data;
                delete old_head;  // 删除旧的哑节点
                size.fetch_sub(1, std::memory_order_relaxed);
                return result;
            }
            
            // 如果CAS失败，则重新加载head
            old_head = head.load(std::memory_order_relaxed);
        }
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
        auto item = queue.pop();
        std::cout << "Consumed: " << *item << std::endl;
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

