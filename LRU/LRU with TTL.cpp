#include <iostream>
#include <unordered_map>
#include <chrono>
#include <thread>


class Node {
public:
	int key, val;
	std::chrono::time_point<std::chrono::steady_clock> timestamp;
	Node *prev = nullptr, *next = nullptr;
	Node (int k = 0, int v = 0): key(k), val(v), timestamp(std::chrono::steady_clock::now()){ }
};

class LRUCache {
public:
	LRUCache(int capacity, int ttl): capacity(capacity), ttl(ttl) {
		dummy = new Node();
		dummy->prev = dummy;
		dummy->next = dummy;
	}

	int get(int key) {
		auto node = getNode(key);
		if (node == nullptr) {
			return -1;
		}
		return node->val;
	}
	
	void put(int key, int value) {
		auto node = getNode(key);
		if (node != nullptr) {
			node->val = value;
			node->timestamp = std::chrono::steady_clock::now();
			return;
		}

		node = new Node(key, value);
		keyToNode[key] = node;
		pushToFront(node);
		if (keyToNode.size() > this->capacity) {
			auto last = dummy->prev;
			keyToNode.erase(last->key);
			remove(last);
			delete last;
		}

		return;
	}

private:
	int capacity;
	std::chrono::seconds ttl;
	std::unordered_map<int, Node*> keyToNode;
	Node *dummy;

	void remove(Node *x) {
		if (x != nullptr) {
			x->prev->next = x->next;
			x->next->prev = x->prev;
		}
	}

	void pushToFront(Node *x) {
		if (x != nullptr) {
			x->prev = dummy;
			x->next = dummy->next;
			x->prev->next = x;
			x->next->prev = x;
		}
	}

	Node* getNode(int key) {
		auto it = keyToNode.find(key);
		if (it == keyToNode.end()) {
			return nullptr;
		}
		auto node = it->second;

		if (std::chrono::steady_clock::now() - node->timestamp > ttl) {
			remove(node);
			keyToNode.erase(it);
			delete node;
			return nullptr;
		}

		node->timestamp = std::chrono::steady_clock::now();
		remove(node);
		pushToFront(node);
		
		return node;
	}
};

int main() {
	using namespace std;
	LRUCache* cache = new LRUCache(2, 5); 


	cout << "Test Case 1:" << endl;
	cache->put(1, 1);
	cache->put(2, 2);
	cout << "Get 1: " << cache->get(1) << " (Expected: 1)" << endl; 
	cache->put(3, 3);
	cout << "Get 2: " << cache->get(2) << " (Expected: -1)" << endl; 
	cache->put(4, 4);
	cout << "Get 1: " << cache->get(1) << " (Expected: -1)" << endl; 
	cout << "Get 3: " << cache->get(3) << " (Expected: 3)" << endl; 
	cout << "Get 4: " << cache->get(4) << " (Expected: 4)" << endl; 


	cout << "Test Case 2:" << endl;
	cache->put(5, 5);
	cout << "Get 3: " << cache->get(3) << " (Expected: -1)" << endl; 
	cout << "Get 4: " << cache->get(4) << " (Expected: 4)" << endl; 
	cout << "Get 5: " << cache->get(5) << " (Expected: 5)" << endl; 


	std::this_thread::sleep_for(std::chrono::seconds(6)); 
	cout << "Get 4 after TTL expiration: " << cache->get(4) << " (Expected: -1)" << endl; 

	delete cache;
	return 0;
}
