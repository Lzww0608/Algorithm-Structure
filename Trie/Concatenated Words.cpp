/*
Given an array of strings words (without duplicates), 
return all the concatenated words in the given list of words.
A concatenated word is defined as a string 
that is comprised entirely of at least two shorter words (not necessarily distinct) in the given array.
*/


#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <iostream>

class Trie {
public:
	bool isEnd;
	std::vector<Trie*> child;
	Trie(int sz) : isEnd(false) {
		child.resize(sz, nullptr);
	}

	~Trie() {
		for (Trie* node : child) {
			if (node) {
				delete node;
			}
		}
	}
};

class Solution {
private:
	Trie* root;

	bool dfs(const std::string& s, int start) {
		auto cur = root;
		for (int i = start; i < s.length(); ++i) {
			int x = s[i] - 'a';
			if (cur->child[x] == nullptr) {
				return false;
			}
			cur = cur->child[x];
			if (cur->isEnd && (i + 1 == s.length() || dfs(s, i + 1))) {
				return true;
			}
		}
		return cur->isEnd;
	}

public:
	std::vector<std::string> findAllConcatenatedWordsInADict(std::vector<std::string>& words) {
		std::vector<std::string> ans;
		int n = words.size();
		root = new Trie(26);
		std::sort(words.begin(), words.end(), [&](const auto& x, const auto& y) {
			return x.size() < y.size();
		});

		for (auto &s : words) {
			if (s.empty()) continue;
			if (dfs(s, 0)) {
				ans.push_back(s);
				continue;
			}

			auto cur = root;
			for (int i = 0; i < s.length(); ++i) {
				int x = s[i] - 'a';
				if (cur->child[x] == nullptr) {
					cur->child[x] = new Trie(26);
				}
				cur = cur->child[x];
			}
			cur->isEnd = true;
		}

		delete root;  
		return ans;
	}
};



int main() {
	Solution solution;

	// 测试样例 1
	std::vector<std::string> words1 = { "cat", "cats", "catsdogcats", "dog", "dogcatsdog", "hippopotamuses", "rat", "ratcatdogcat" };
	std::vector<std::string> result1 = solution.findAllConcatenatedWordsInADict(words1);
	std::cout << "Test Case 1:\n";
	for (const auto& word : result1) {
		std::cout << word << "\n";
	}

	// 测试样例 2
	std::vector<std::string> words2 = { "cat", "dog", "catdog" };
	std::vector<std::string> result2 = solution.findAllConcatenatedWordsInADict(words2);
	std::cout << "Test Case 2:\n";
	for (const auto& word : result2) {
		std::cout << word << "\n";
	}

	// 测试样例 3
	std::vector<std::string> words3 = { "", "a", "b", "ab", "abc" };
	std::vector<std::string> result3 = solution.findAllConcatenatedWordsInADict(words3);
	std::cout << "Test Case 3:\n";
	for (const auto& word : result3) {
		std::cout << word << "\n";
	}

	return 0;
}