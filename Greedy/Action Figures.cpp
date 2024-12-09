/*
https://codeforces.com/problemset/problem/2026/C
*/

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdint>

void solve() {
    int64_t n = 0;
    std::string s;
    std::cin >> n >> s;

    int64_t ans = n * (n + 1) / 2, need = 0;
    for (int64_t i = n - 1; i >= 0; --i) {
        if (s[i] == '1' && need + 1 <= i) {
            ans -= i + 1;
            need++;
        } else {
            need = std::max(need - 1, int64_t(0));
        }
    } 
    std::cout << ans << '\n';
}


int main() {
    int t = 0;
    std::cin >> t;
    while (t--) {
        solve();    
    }
}