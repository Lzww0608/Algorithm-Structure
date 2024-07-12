#include <iostream>
#include <vector>
#include <cstdlib>

constexpr int LEVEL = 8;

class Node {
public:
	int val;
	std::vector<Node*> next;
	Node(int v) : val(v) {
		next.resize(LEVEL, nullptr);
	}
};

class Skiplist {
public:
	Skiplist() {
		head = new Node(-1);
	}

	~Skiplist() {
		delete head;
	}

	void find(int target, std::vector<Node*>& pre) {
		auto p = head;
		for (int i = LEVEL - 1; i >= 0; i--) {
			while (p->next[i] && p->next[i]->val < target) {
				p = p->next[i];
			}
			pre[i] = p;
		}
	}

	bool search(int target) {
		std::vector<Node*> pre(LEVEL);
		find(target, pre);
		auto p = pre[0]->next[0];
		return p != nullptr && p->val == target;
	}

	void add(int num) {
		std::vector<Node*> pre(LEVEL);
		find(num, pre);

		auto p = new Node(num);
		for (int i = 0; i < LEVEL; ++i) {
			p->next[i] = pre[i]->next[i];
			pre[i]->next[i] = p;
			if (rand() & 1) break;
		}
	}

	bool erase(int num) {
		std::vector<Node*> pre(LEVEL);
		find(num, pre);

		auto p = pre[0]->next[0];
		if (p == nullptr || p->val != num) {
			return false;
		}

		for (int i = 0; i < LEVEL && pre[i]->next[i] == p; ++i) {
			pre[i]->next[i] = p->next[i];
		}

		delete p;

		return true;
	}

private:
	Node *head;
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
