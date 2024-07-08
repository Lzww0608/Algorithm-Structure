#include <iostream>
#include <unordered_map>
using namespace std;

class Node {
public:
	int key, val;
	Node *prev, *next;
	Node(int k = 0, int v = 0) : key(k), val(v) {
	}
};

class LRUCache {
private:
	int capacity;
	std::unordered_map<int, Node*> keyToNode;
	Node* dummy;
public:
	void remove(Node *x) {
		x->prev->next = x->next;
		x->next->prev = x->prev;
	}

	void pushFront(Node *x) {
		x->prev = dummy;
		x->next = dummy->next;
		x->prev->next = x;
		x->next->prev = x;
	}

	Node* getNode(int key) {
		auto it = keyToNode.find(key);
		if (it == keyToNode.end()) {
			return nullptr;
		}
		auto node = it->second;
		remove(node);
		pushFront(node);
		return node;
	}

	LRUCache(int capacity) {
		this->capacity = capacity;
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
			return;
		}

		node = new Node(key, value);
		keyToNode[key] = node;
		pushFront(node);
		if (keyToNode.size() > this->capacity) {
			auto last = dummy->prev;
			remove(last);
			keyToNode.erase(last->key);
			delete last;
		}
	}
};

int main() {
	LRUCache* cache = new LRUCache(2);

	// Test Case 1
	cout << "Test Case 1:" << endl;
	cache->put(1, 1);
	cache->put(2, 2);
	cout << "Get 1: " << cache->get(1) << " (Expected: 1)" << endl; // returns 1
	cache->put(3, 3); 
	cout << "Get 2: " << cache->get(2) << " (Expected: -1)" << endl; // returns -1 (not found)
	cache->put(4, 4); 
	cout << "Get 1: " << cache->get(1) << " (Expected: -1)" << endl; // returns -1 (not found)
	cout << "Get 3: " << cache->get(3) << " (Expected: 3)" << endl; // returns 3
	cout << "Get 4: " << cache->get(4) << " (Expected: 4)" << endl; // returns 4

	// Test Case 2
	cout << "Test Case 2:" << endl;
	cache->put(5, 5);
	cout << "Get 3: " << cache->get(3) << " (Expected: -1)" << endl; // returns -1 (evicted)
	cout << "Get 4: " << cache->get(4) << " (Expected: 4)" << endl; // returns 4
	cout << "Get 5: " << cache->get(5) << " (Expected: 5)" << endl; // returns 5

	delete cache;
	return 0;
}
