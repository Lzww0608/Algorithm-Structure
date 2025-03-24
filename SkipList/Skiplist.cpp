#include <iostream>
#include <vector>
#include <cstdlib>

Skiplist {
private:
    static constexpr int kMaxHeight = 8;

    struct Node {
        int val;
        int height;
        Node **next;
        Node(int v = 0, int h = kMaxHeight): val(v), height(h) {
            next = new Node*[h];
            while (--h >= 0) {
                next[h] = nullptr;
            }
        }
    };

    int getRandomHeight() {
        int h = 1;
        while (h <= kMaxHeight && rand() % 4 == 1) {
            h++;
        }
        return h;
    }

    Node* findEqualOrGreater(int target, Node **prev) {
        auto it = head;
        int level = kMaxHeight - 1;
        while (true) {
            auto next = it->next[level];
            if (next != nullptr && next->val < target) {
                it = next;
            } else {
                if (prev != nullptr) {
                    prev[level] = it;
                } 

                if (level == 0) {
                    return next;
                } else {
                    --level;
                }
            }
        }

        return nullptr;
    }
    

    Node* head;
public:
    Skiplist() {
        head = new Node();
    }
    
    bool search(int target) {
        auto it = findEqualOrGreater(target, nullptr);
        return it != nullptr && it->val == target;
    }
    
    void add(int num) {
        auto prev = new Node*[kMaxHeight];
        auto node = new Node(num, getRandomHeight());
        findEqualOrGreater(num, prev);
        for (int i = 0; i < node->height; i++) {
            node->next[i] = prev[i]->next[i];
            prev[i]->next[i] = node;
        }
    }
    


    vector<int> rangeQuery(int start, int end) {
        vector<int> res;
        Node* cur = findEqualOrGreater(start, nullptr);
        while (cur && cur->next[0] && cur->next[0]->val <= end) {
            cur = cur->next[0];  
            res.push_back(cur->val);
        }
        return res;
    }
    
    ~Skiplist() {  
        Node* cur = head;
        while (cur) {
            Node* tmp = cur->next[0];
            delete cur;
            cur = tmp;
        }
    }

    bool erase(int num) {
        Node* prev[kMaxHeight] = {nullptr};  
        Node* to_del = findEqualOrGreater(num, prev);
        if (!to_del || to_del->val != num) return false;

        for (int i=0; i<to_del->height; ++i) 
            prev[i]->next[i] = to_del->next[i];
        
        delete to_del;  
        return true;
    }
};


int main() {
	Skiplist skiplist;

	// 测试添加
	skiplist.add(1);
	skiplist.add(3);
	skiplist.add(7);
	skiplist.add(8);
	skiplist.add(9);

	// 测试搜索
	std::cout << "Search 3: " << skiplist.search(3) << std::endl; // 应该输出 1 (true)
	std::cout << "Search 5: " << skiplist.search(5) << std::endl; // 应该输出 0 (false)

	// 测试删除
	std::cout << "Erase 3: " << skiplist.erase(3) << std::endl; // 应该输出 1 (true)
	std::cout << "Search 3: " << skiplist.search(3) << std::endl; // 应该输出 0 (false)

	// 再次测试删除一个不存在的元素
	std::cout << "Erase 5: " << skiplist.erase(5) << std::endl; // 应该输出 0 (false)

	return 0;
}
