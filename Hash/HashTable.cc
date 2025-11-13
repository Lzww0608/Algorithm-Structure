#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <stdexcept>

template <typename K, typename V, typename Hash = std::hash<std::string>> 
class HashTable {
public:
    explicit HashTable(size_t initial_buckets = 16, double max_lf = 0.75f)
        : num_elements(0), max_load_factor(max_lf) {
            if (initial_buckets == 0) {
                initial_buckets = 1;
            }

            buckets.resize(initial_buckets, nullptr);
    }
    ~HashTable() {
        clear();
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    std::pair<V*, bool> insert(const K& key, const V& val) {
        if ((num_elements + 1.0) / buckets.size() > max_load_factor) {
            rehash();
        }

        size_t index = get_bucket_index(key);
        Node* head = buckets[index];
        Node* current = head;

        while (current != nullptr) {
            if (current->key == key) {
                current->value = val;
                return {&(current->value), false};
            }

            current = current->next;
        }

        Node* newNode = new Node(key, val);
        newNode->next = head;
        buckets[index] = newNode;
        num_elements++;

        return {&(newNode->value), true};
    }

    V& operator[](const K& key) {
        size_t index = get_bucket_index(key);
        Node* current = buckets[index];
        while (current != nullptr) {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        }

        return *(insert(key, V{}).first);
    }

    bool erase(const K& key) {
        size_t index = get_bucket_index(key);
        Node* current = buckets[index];
        Node* prev = nullptr;

        while (current != nullptr) {
            if (current->key == key) {
                if (prev == nullptr) {
                    buckets[index] = current->next;
                } else {
                    prev->next = current->next;
                }

                delete current;
                num_elements--;
                return true;
            }

            prev = current;
            current = current->next;
        }

        return false;
    }

    bool contains(const K& key) const {
        size_t index = get_bucket_index(key);
        Node* current = buckets[index];
        if (current != nullptr) {
            if (current->key == key) {
                return true;
            }
            current = current->next;
        }

        return false;
    }

    void clear() {
        for (size_t i = 0; i < buckets.size(); i++) {
            Node* current = buckets[i];
            while (current != nullptr) {
                auto next = current->next;
                delete current;
                current = next;
            }
            buckets[i] = nullptr;
        }

        num_elements = 0;
    }

    size_t size() const {
        return num_elements;
    }

    bool empty() const {
        return num_elements == 0;
    }

private:
    struct Node {
        K key;
        V value;
        Node* next;

        Node(const K& k, const V& v): key(k), value(v), next(nullptr) {}
    };
    

    std::vector<Node*> buckets;
    size_t num_elements;
    double max_load_factor;
    Hash hasher;

    size_t get_bucket_index(const K& key) const {
        return hasher(key) % buckets.size();
    }

    void rehash() {
        size_t old_bucket_count = buckets.size();
        size_t new_bucket_count = old_bucket_count * 2;
        if (new_bucket_count == 0) new_bucket_count = 1;

        std::vector<Node*> new_buckets(new_bucket_count, nullptr);
        for (size_t i = 0; i < old_bucket_count; i++) {
            Node* current = buckets[i];
            while (current != nullptr) {
                Node* next_node = current->next;
                size_t new_index = hasher(current->key) % new_bucket_count;

                current->next = new_buckets[new_index];
                new_buckets[new_index] = current;

                current = next_node;
            }
        }

        buckets.swap(new_buckets);
    }
};