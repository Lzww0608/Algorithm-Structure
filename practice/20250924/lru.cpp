

#include <unordered_map>

struct Node {
    int key, val;
    Node *prev, *next;
    Node(int k = 0, int v = 0) : key(k), val(v) {
        prev = nullptr;
        next = nullptr;
    }
};

class LRUCache {
public:
    LRUCache(int capacity): capacity(capacity) {
        dummy = new Node();
        dummy->next = dummy;
        dummy->prev = dummy;
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
        if (keyToNode.size() == capacity) {
            auto last = dummy->prev;
            remove(last);
            keyToNode.erase(last->key);
            delete last;
        }
        keyToNode[key] = node;
        pushFront(node);
    }

private:
    int capacity;
    Node* dummy;
    std::unordered_map<int, Node*> keyToNode;

    void remove(Node* node) {
        if (node != nullptr) {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
    }

    void pushFront(Node *node) {
        if (node != nullptr) {
            node->prev = dummy;
            node->next = dummy->next;
            node->prev->next = node;
            node->next->prev = node;
        }
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
};