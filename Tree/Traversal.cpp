#include <iostream>
#include <vector>
#include <queue>

struct TreeNode {
	int val;
	TreeNode *left;
	TreeNode *right;
	TreeNode() : val(0), left(nullptr), right(nullptr) {
	}
	TreeNode(int x) : val(x), left(nullptr), right(nullptr) {
	}
	TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {
	}
};

std::vector<int> levelOrder(TreeNode* root) {
	std::vector<int> ans;
	std::queue<TreeNode*> q;
	if (root != nullptr) {
		q.push(root);
	}

	while (!q.empty()) {
		auto node = q.front();
		q.pop();
		ans.push_back(node->val);
		if (node->left != nullptr) {
			q.push(node->left);
		}
		if (node->right != nullptr) {
			q.push(node->right);
		}
	}

	return ans;
}

void preOrder(TreeNode* root, std::vector<int> &ans) {
	if (root == nullptr) return;

	ans.push_back(root->val);
	preOrder(root->left, ans);
	preOrder(root->right, ans);
}

void inOrder(TreeNode* root, std::vector<int> &ans) {
	if (root == nullptr) return;

	inOrder(root->left, ans);
	ans.push_back(root->val);
	inOrder(root->right, ans);
}

void postOrder(TreeNode* root, std::vector<int> &ans) {
	if (root == nullptr) return;

	postOrder(root->left, ans);
	postOrder(root->right, ans);
	ans.push_back(root->val);
}

int main() {
	//         1
	//        / \
    //       2   3
	//      / \
    //     4   5
	TreeNode* root = new TreeNode(1);
	root->left = new TreeNode(2);
	root->right = new TreeNode(3);
	root->left->left = new TreeNode(4);
	root->left->right = new TreeNode(5);

	// 层次遍历
	std::vector<int> levelOrderResult = levelOrder(root);
	std::cout << "Level Order: ";
	for (int val : levelOrderResult) {
		std::cout << val << " ";
	}
	std::cout << std::endl;

	// 前序遍历
	std::vector<int> preOrderResult;
	preOrder(root, preOrderResult);
	std::cout << "Pre Order: ";
	for (int val : preOrderResult) {
		std::cout << val << " ";
	}
	std::cout << std::endl;

	// 中序遍历
	std::vector<int> inOrderResult;
	inOrder(root, inOrderResult);
	std::cout << "In Order: ";
	for (int val : inOrderResult) {
		std::cout << val << " ";
	}
	std::cout << std::endl;

	// 后序遍历
	std::vector<int> postOrderResult;
	postOrder(root, postOrderResult);
	std::cout << "Post Order: ";
	for (int val : postOrderResult) {
		std::cout << val << " ";
	}
	std::cout << std::endl;

	// 释放内存
	delete root->left->left;
	delete root->left->right;
	delete root->left;
	delete root->right;
	delete root;

	return 0;
}
