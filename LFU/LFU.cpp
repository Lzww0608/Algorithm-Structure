#include <iostream>
#include <unordered_map>


class Node {
public:
	int key, val, freq = 1;
	Node *prev, *next;
	Node(int k = 0, int v = 0) : key(k), val(v) {
	}
};

class LFUCache {
private:
	int capacity;
	int min_freq = 1;
	std::unordered_map<int, Node*> keyToNode;
	std::unordered_map<int, Node*> freqToList;

	void remove(Node *x) {
		x->prev->next = x->next;
		x->next->prev = x->prev;
	}

	Node* newList() {
		auto dummy = new Node();
		dummy->prev = dummy;
		dummy->next = dummy;
		return dummy;
	}

	void pushFront(int freq, Node *x) {
		auto it = freqToList.find(freq);
		if (it == freqToList.end()) {
			it = freqToList.emplace(freq, newList()).first;
		}
		auto dummy = it->second;
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
		auto dummy = freqToList[node->freq];
		if (dummy->prev == dummy) {
			delete dummy;
			freqToList.erase(node->freq);
			if (node->freq == min_freq) {
				min_freq++;
			}
		}
		pushFront(++node->freq, node);
		return node;
	}
public:
	LFUCache(int capacity) {
		this->capacity = capacity;
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
		if (keyToNode.size() == this->capacity) {
			auto dummy = freqToList[min_freq];
			auto last = dummy->prev;
			remove(last);
			keyToNode.erase(last->key);
			delete last;
			if (dummy->prev == dummy) {
				delete dummy;
				freqToList.erase(min_freq);
			}
		}
		min_freq = 1;
		keyToNode[key] = node;
		pushFront(1, node);
	}
};

int main() {
	LFUCache* cache = new LFUCache(2);

	using namespace std;
	// Test Case 1
	cout << "Test Case 1:" << endl;
	cache->put(1, 1);
	cache->put(2, 2);
	cout << "Get 1: " << cache->get(1) << " (Expected: 1)" << endl; // returns 1
	cache->put(3, 3); 
	cout << "Get 2: " << cache->get(2) << " (Expected: -1)" << endl; // returns -1 (not found)
	cache->put(4, 4); 
	cout << "Get 1: " << cache->get(1) << " (Expected: 1)" << endl; // returns -1 (not found)
	cout << "Get 3: " << cache->get(3) << " (Expected: -1)" << endl; // returns 3
	cout << "Get 4: " << cache->get(4) << " (Expected: 4)" << endl; // returns 4

	// Test Case 2
	cout << "Test Case 2:" << endl;
	cache->put(5, 5);
	cout << "Get 3: " << cache->get(3) << " (Expected: -1)" << endl; // returns -1 (evicted)
	cout << "Get 4: " << cache->get(4) << " (Expected: -1)" << endl; // returns 4
	cout << "Get 5: " << cache->get(5) << " (Expected: 5)" << endl; // returns 5

	delete cache;
	return 0;
}
