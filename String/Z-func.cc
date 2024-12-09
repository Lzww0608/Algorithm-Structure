#include "Z-func.h"

#include <algorithm>
#include <vector>
#include <string>

std::vector<int> z_func(const std::string& s) {
    int n = s.length();
    std::vector<int> z(n);
    for (int i = 1, l = 0, r = 0; i < n; ++i) {
        if (i <= r)
            z[i] = std::max(std::min(r - i + 1, z[i - l]), 0);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            l = i;
            r = i + z[i];
            ++z[i];
        }
    }
    return z;
} 