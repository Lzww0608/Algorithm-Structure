/*
题目：
学校外在卖大饼，这些饼有大有小，现在你的一个手上有一叠饼，大小不一样，
你每次只能从这叠饼中抽出一张放在最上面，求最少需要抽多少次才可以让这叠饼从小到大有序。

思路：
1. 由暴力法可以简单推测出，每张饼最多抽取一次，抽取次数x一定大于等于0小于n（饼的个数）。
2. 逆向思维，最少抽取多少次也就是最少抽取多少张饼，我们可以改为求最多有多少张饼不需要移动，最少抽取次数为n-最多不需要移动的饼数。
3. 从后向前遍历，求以n结尾的最长连续递增子序列。
*/

#include <iostream>
#include <vector>

int minMovesToSort(std::vector<int>& arr) {
    int n = arr.size();
    int need = n;
    int suf = 0; 
    for (int i = n - 1; i >= 0; --i) {
        if (arr[i] == need) {
            suf++;
            need--;
        }
    }
    return n - suf;
}

int main() {
    std::vector<int> arr = {3, 1, 2, 4, 5};
    std::cout << minMovesToSort(arr) << std::endl;
    return 0;
}