/*
You are given a string `s` of even length n. 
String `s` is binary, in other words, consists only of 0's and 1's.
String `s` has exactly n/2 zeroes and n/2 ones (n is even).

In one operation you can reverse any substring of `s`. 
A substring of a string is a contiguous subsequence of that string.

What is the minimum number of operations you need to make string `s` alternating?
*/
#include <iostream>
#include <string>

void solve() {
	int n;
	std::cin >> n;
	std::string s;
	std::cin >> s;
	int ans = 0;
	for (int i = 0; i < n; ++i) {
		if (s[i] == s[(i + 1) % n]) {
			ans++;
		}
	}
	std::cout << ans / 2 << '\n';
}


int main() {
	int t;
	std::cin >> t;
	while (t--) {
		solve();
	}
	
	return 0;
}

/*
input:
3
2
10
4
0110
8
11101000

output:
0
1
2
*/