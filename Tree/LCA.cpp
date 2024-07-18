#include <iostream>

struct TreeNode {
	int val;
	TreeNode *left, *right;
	TreeNode(int x) : val(x), left(nullptr), right(nullptr) {
	}
};

TreeNode* LCA(TreeNode* root, TreeNode* p, TreeNode* q) {
	if (root == nullptr || root == p || root == q) {
		return root;
	}

	auto l = LCA(root->left, p, q);
	auto r = LCA(root->right, p, q);

	if (l != nullptr) {
		if (r != nullptr) {
			return root;
		}
		return l;
	}

	return r;
}


int main() {

	TreeNode* root = new TreeNode(3);
	root->left = new TreeNode(5);
	root->right = new TreeNode(1);
	root->left->left = new TreeNode(6);
	root->left->right = new TreeNode(2);
	root->right->left = new TreeNode(0);
	root->right->right = new TreeNode(8);
	root->left->right->left = new TreeNode(7);
	root->left->right->right = new TreeNode(4);


	TreeNode* p = root->left; // 节点5
	TreeNode* q = root->left->right->right; // 节点4

	TreeNode* ancestor = LCA(root, p, q);

	if (ancestor != nullptr) {
		std::cout << "The Lowest Common Ancestor of " << p->val << " and " << q->val << " is " << ancestor->val << std::endl;
	}
	else {
		std::cout << "No Common Ancestor found." << std::endl;
	}

	delete root->left->right->right;
	delete root->left->right->left;
	delete root->right->right;
	delete root->right->left;
	delete root->left->right;
	delete root->left->left;
	delete root->right;
	delete root->left;
	delete root;

	return 0;
}
