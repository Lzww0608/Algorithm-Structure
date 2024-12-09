#include "KMP.h"

#include <vector>
#include <string>

std::vector<int> computePi(const std::string& pattern) {
    int n = pattern.size();
    std::vector<int> pi(n);
    int cnt = 0;

    for (int i = 1; i < n; ++i) {
        while (cnt > 0 && pattern[cnt] != pattern[i]) {
            cnt = pi[cnt - 1];
        }
        if (pattern[cnt] == pattern[i]) {
            cnt++;
        }
        pi[i] = cnt;
    }

    return pi;
}

std::vector<int> kmp(const std::string& text, const std::string& pattern) {
    std::vector<int> pi = computePi(pattern);
    int n = text.size(), m = pattern.size();
    std::vector<int> ans;

    for (int i = 0, cnt = 0; i < n; ++i) {
        while (cnt > 0 && text[i] != pattern[cnt]) {
            cnt = pi[cnt - 1];
        }
        if (text[i] == pattern[cnt]) {
            cnt++;
        }
        if (cnt == m) {
            ans.push_back(i - m + 1);
            cnt = pi[cnt - 1];
        }
    }

    return ans;
}
