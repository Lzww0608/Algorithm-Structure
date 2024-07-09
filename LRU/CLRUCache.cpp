/*
实现线程安全以及高并发的LRU：
1. 通过对所有的Node操作加锁实现线程安全
2. 为了减小锁的粒度，采用分桶操作，同时可以均衡访问负载
3. 使用条件变量和任务队列来异步调用Node操作，以实现高并发
TODO:
	将单一工作线程改为线程池以进一步提高并发性能。
*/

#include <string>
#include <any>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <iostream>
#include <utility>
#include <memory>
#include <queue>
#include <functional>
#include <condition_variable>
#include <thread>
#include <atomic>


class Node {
public:
	std::string key;
	std::any value;
	Node *prev, *next;
	Node(const std::string &k = " ", const std::any v = nullptr)
		: key(k), value(v), prev(nullptr), next(nullptr) { }
};

class Bucket {
private:
	std::unordered_map<std::string, Node*> keys;
	std::mutex mtx;

public:
	Node* get(const std::string& key) {
		std::lock_guard<std::mutex> lock(mtx);
		auto it = keys.find(key);
		if (it != keys.end()) {
			return it->second;
		}
		return nullptr;
	}

	Node *set(const std::string& key, std::any value) {
		std::lock_guard<std::mutex> lock(mtx);
		auto it = keys.find(key);
		if (it != keys.end()) {
			auto node = it->second;
			node->value = value;
			return node;
		}
		auto node = new Node(key, value);
		keys[key] = node;
		return node;
	}

	void update(const std::string& key, Node* node) {
		std::lock_guard<std::mutex> lock(mtx);
		keys[key] = node;
	}

	void del(const std::string &key) {
		std::lock_guard<std::mutex> lock(mtx);
		keys.erase(key);
	}

	void clear() {
		std::lock_guard<std::mutex> lock(mtx);
		keys.clear();
	}
};

class CLRUCache {
private:
	std::vector<std::shared_ptr<Bucket>> buckets;
	uint32_t bucketMask;

	int cap;
	Node *dummy;
	std::queue<std::function<void()>> tasks;

	std::mutex mtx;
	std::mutex task_mtx;
	std::condition_variable task_cv;
	std::thread worker_thread;
	std::atomic<bool> stop_flag;

	void remove(Node *node) {
		if (node && node->prev && node->next) {
			node->prev->next = node->next;
			node->next->prev = node->prev;
		}
	}

	void pushFront(Node *node) {
		if (node) {
			node->next = dummy->next;
			node->prev = dummy;
			node->prev->next = node;
			node->next->prev = node;
		}
	}

	void worker() {
		while (!stop_flag) {
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(task_mtx);
				task_cv.wait(lock, [&] {return !tasks.empty() || stop_flag;});
				if (tasks.empty()) break;
				task = std::move(tasks.front());
				tasks.pop();
			}
			task();
		}
	}

	std::shared_ptr<Bucket> getBucket(const std::string& key) {
		std::hash<std::string> hash_fn;
		return buckets[hash_fn(key) & bucketMask];
	}

public:
	CLRUCache(int capacity)
		: cap(capacity), bucketMask(1024 - 1), stop_flag(false) {
		dummy = new Node();
		dummy->next = dummy;
		dummy->prev = dummy;
		for (int i = 0; i < 1024; ++i) {
			buckets.push_back(std::make_shared<Bucket>());
		}
		worker_thread = std::thread(&CLRUCache::worker, this);
	}

	~CLRUCache() {
		stop_flag = true;
		task_cv.notify_all();
		if (worker_thread.joinable()) {
			worker_thread.join();
		}

		auto cur = dummy->next;
		while (cur != dummy) {
			auto next = cur->next;
			delete cur;
			cur = next;
		}
		delete dummy;
	}

	int size() {
		int cnt = 0;
		auto cur = dummy->next;
		while (cur != dummy) {
			cur = cur->next;
			++cnt;
		}
		return cnt;
	}

	void moveFront(Node *node) {
		std::lock_guard<std::mutex> lock(mtx);
		remove(node);
		pushFront(node);
	}

	void async(std::function<void()> task) {
		{
			std::lock_guard<std::mutex> lock(task_mtx);
			tasks.push(std::move(task));
		}
		task_cv.notify_one();
	}

	std::any get(const std::string& key) {
		auto bucket = getBucket(key);
		auto node = bucket->get(key);
		if (node) {
			async([this, node] {moveFront(node);});
			return node->value;
		}
		return nullptr;
	}

	void put(const std::string &key, std::any value) {
		auto bucket = getBucket(key);
		auto node = bucket->set(key, value);
		async([this, node] { moveFront(node); });

		async([this, bucket] {
			if (size() > cap) {
				auto last = dummy->prev;
				if (last != dummy) {
					remove(last);
					bucket->del(last->key);
					delete last;
				}
			}
		});
	}

	void del(const std::string &key) {
		auto bucket = getBucket(key);
		auto node = bucket->get(key);
		if (node) {
			async([this, node] {
				remove(node);
				delete node;
			});
			bucket->del(key);
		}
	}

	void clear() {
		std::unique_lock<std::mutex> lock(mtx);
		for (auto &bucket : buckets) {
			bucket->clear();
		}
		auto cur = dummy->next;
		while (cur != dummy) {
			auto next = cur->next;
			delete cur;
			cur = next;
		}
		dummy->next = dummy;
		dummy->prev = dummy;
		//cv.notify_all();
	}
};

int main() {
	CLRUCache cache(10);

	cache.async([&cache] { cache.put("test", 42); });
	cache.async([&cache] {
		auto value = std::any_cast<int>(cache.get("test"));
		std::cout << "value: " << value << std::endl;
	});

	std::this_thread::sleep_for(
		std::chrono::seconds(1));  // wait for async tasks to complete

	return 0;
}