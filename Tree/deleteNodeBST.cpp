#include <iostream>
#include <queue>

struct TreeNode {
	int val;
	TreeNode *left, *right;
	TreeNode(int x = 0, TreeNode *left = nullptr, TreeNode *right = nullptr): val(x), left(left), right(right) { }
};

TreeNode* deleteNode(TreeNode* root, int key) {
	TreeNode** p = &root;
	while (*p != nullptr && (*p)->val != key) {
		if ((*p)->val > key) {
			p = &(*p)->left;
		}
		else {
			p = &(*p)->right;
		}
	}

	if ((*p) == nullptr) return root;

	TreeNode** t = &(*p)->right;
	while ((*t) != nullptr) {
		t = &(*t)->left;
	}

	*t = (*p)->left;
	*p = (*p)->right;

	return root;
}

TreeNode* deleteNode_dfs(TreeNode* root, int key) {
	if (root == nullptr) return root;

	if (root->val < key) {
		root->right = deleteNode_dfs(root->right, key);
	}
	else if (root->val > key) {
		root->left = deleteNode_dfs(root->left, key);
	}
	else {
		if (root->left == nullptr) {
			auto tmp = root->right;
			delete root;
			return tmp;
		}
		else if (root->right == nullptr) {
			auto tmp = root->left;
			delete root;
			return tmp;
		}

		auto minNode = root->right;
		while (minNode->left != nullptr) {
			minNode = minNode->left;
		}
		root->val = minNode->val;
		root->right = deleteNode(root->right, minNode->val);
	}

	return root;
}

void printInOrder(TreeNode* root) {
	if (root == nullptr) return;
	printInOrder(root->left);
	std::cout << root->val << " ";
	printInOrder(root->right);
}

void printLevelOrder(TreeNode* root) {
	if (root == nullptr) return;
	std::queue<TreeNode*> q;
	q.push(root);
	while (!q.empty()) {
		TreeNode* node = q.front();
		q.pop();
		std::cout << node->val << " ";
		if (node->left) q.push(node->left);
		if (node->right) q.push(node->right);
	}
	std::cout << std::endl;
}


int main() {
	TreeNode* root = new TreeNode(5);
	root->left = new TreeNode(3);
	root->right = new TreeNode(6);
	root->left->left = new TreeNode(2);
	root->left->right = new TreeNode(4);
	root->right->right = new TreeNode(7);

	std::cout << "Original Tree (In-Order Traversal): ";
	printInOrder(root);
	std::cout << std::endl;

	std::cout << "Original Tree (Level-Order Traversal): ";
	printLevelOrder(root);
	std::cout << std::endl;

	int key = 3;
	std::cout << "Deleting node with key " << key << std::endl;
	root = deleteNode(root, key);

	std::cout << "Tree after deletion (In-Order Traversal): ";
	printInOrder(root);
	std::cout << std::endl;

	std::cout << "Tree after deletion (Level-Order Traversal): ";
	printLevelOrder(root);
	std::cout << std::endl;

	key = 6;
	std::cout << "Deleting node with key " << key << std::endl;
	root = deleteNode(root, key);

	std::cout << "Tree after deletion (In-Order Traversal): ";
	printInOrder(root);
	std::cout << std::endl;

	std::cout << "Tree after deletion (Level-Order Traversal): ";
	printLevelOrder(root);
	std::cout << std::endl;

	return 0;
}