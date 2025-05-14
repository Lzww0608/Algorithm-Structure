#include <iostream>
#include <unordered_map>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

struct Node {
    int key, val;
    Node* next, *prev;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;

    Node(int k = 0, int v = 0) : key(k), val(v), next(nullptr), prev(nullptr), timestamp(std::chrono::steady_clock::now()) {}
};

class LRUCache {
private:
    int capacity;
    int ttl;
    std::unordered_map<int, Node*> keyToNode;
    Node* dummy;

    void remove(Node* x) {
        if (x != nullptr) {
            x->prev->next = x->next;
            x->next->prev = x->prev;
        }
    }

    void pushToFront(Node* x) {
        if (x != nullptr) {
            x->prev = dummy;
            x->next = dummy->next;
            x->prev->next = x;
            x->next->prev = x;
        }
    }

    Node* getNode(int key) {
        auto it = keyToNode.find(key);
        if (it == keyToNode.end()) return nullptr;
        Node* x = it->second;
        if (std::chrono::steady_clock::now() - x->timestamp > std::chrono::seconds(ttl)) {
            remove(x);
            keyToNode.erase(it);
            delete x;
            return nullptr;
        }
        remove(x);
        pushToFront(x);
        x->timestamp = std::chrono::steady_clock::now();
        return x;
    }
    

public:

    LRUCache(int capacity = 1000, int ttl = 3600) : capacity(capacity), ttl(ttl), dummy(new Node()) {
        dummy->next = dummy;
        dummy->prev = dummy;
    }

    ~LRUCache() {
        while (dummy->next != dummy) {
            Node* temp = dummy->next;
            remove(temp);
            keyToNode.erase(temp->key);
            delete temp;
        }
        delete dummy;
    }

    int get(int key) {
        Node* x = getNode(key);
        return x ? x->val : -1;
    }

    void put(int key, int value) {
        Node* x = getNode(key);
        if (x == nullptr) {
            x = new Node(key, value);
            keyToNode[key] = x;
            pushToFront(x);
            if (keyToNode.size() > capacity) {
                Node* last = dummy->prev;
                remove(last);
                keyToNode.erase(last->key);
                delete last;
            }
        } else {
            x->val = value;
        }
        x->timestamp = std::chrono::steady_clock::now();
    }
};


class LRUCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前的设置
    }

    void TearDown() override {
        // 每个测试后的清理
    }
};

TEST_F(LRUCacheTest, BasicOperations) {
    LRUCache cache(2);
    
    cache.put(1, 1);
    cache.put(2, 2);
    EXPECT_EQ(1, cache.get(1));
    
    cache.put(3, 3);  // 这应该会淘汰键2
    EXPECT_EQ(-1, cache.get(2));
    
    cache.put(4, 4);  // 这应该会淘汰键1
    EXPECT_EQ(-1, cache.get(1));
    EXPECT_EQ(3, cache.get(3));
    EXPECT_EQ(4, cache.get(4));
}

TEST_F(LRUCacheTest, UpdateExistingKey) {
    LRUCache cache(2);
    
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(1, 10);  // 更新键1的值
    
    EXPECT_EQ(10, cache.get(1));
    EXPECT_EQ(2, cache.get(2));
}

TEST_F(LRUCacheTest, LRUEvictionPolicy) {
    LRUCache cache(2);
    
    cache.put(1, 1);
    cache.put(2, 2);
    EXPECT_EQ(1, cache.get(1));  // 访问键1，使其成为最近使用的
    
    cache.put(3, 3);  // 这应该会淘汰键2而不是键1
    EXPECT_EQ(1, cache.get(1));
    EXPECT_EQ(-1, cache.get(2));
    EXPECT_EQ(3, cache.get(3));
}

TEST_F(LRUCacheTest, TTLExpiration) {
    LRUCache cache(2, 1);  // TTL设置为1秒
    
    cache.put(1, 1);
    EXPECT_EQ(1, cache.get(1));
    
    // 等待TTL过期
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // 键1应该已经过期
    EXPECT_EQ(-1, cache.get(1));
}

TEST_F(LRUCacheTest, TTLReset) {
    LRUCache cache(2, 2);  // TTL设置为2秒
    
    cache.put(1, 1);
    
    // 等待1秒（未过期）
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 访问键1，应该重置TTL
    EXPECT_EQ(1, cache.get(1));
    
    // 再等待1秒（如果没有重置TTL，键1应该过期）
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 键1应该仍然有效
    EXPECT_EQ(1, cache.get(1));
    
    // 再等待2秒（应该过期）
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // 键1应该已经过期
    EXPECT_EQ(-1, cache.get(1));
}

TEST_F(LRUCacheTest, MixedOperations) {
    LRUCache cache(3, 3);  // 容量为3，TTL为3秒
    
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    
    EXPECT_EQ(1, cache.get(1));
    
    // 等待2秒（未过期）
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    cache.put(4, 4);  // 这应该会淘汰键2（最近最少使用的）
    
    EXPECT_EQ(1, cache.get(1));
    EXPECT_EQ(-1, cache.get(2));
    EXPECT_EQ(3, cache.get(3));
    EXPECT_EQ(4, cache.get(4));
    
    // 再等待2秒（键3应该过期，但键1和键4不应该过期，因为它们被访问过）
    std::this_thread::sleep_for(std::chrono::seconds(4));
    
    EXPECT_EQ(-1, cache.get(1));
    EXPECT_EQ(-1, cache.get(3));  
    EXPECT_EQ(-1, cache.get(4));
}
