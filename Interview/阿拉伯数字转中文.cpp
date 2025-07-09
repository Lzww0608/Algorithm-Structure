#include <iostream>
#include <string>

using namespace std;
// 十万，百万，千万可当做填充位
string Num[] = {"零", "一", "二", "三", "四", "五", "六", "七", "八", "九", "十"};
string Unit[] = {"", "十", "百", "千", "万", "十万", "百万", "千万", "亿"};

string NumberToChinese(int num) {
	string res = "";
	string str = to_string(num); // 直接去掉前导0
	int n = str.length();
	
	for (int i = 0; i < n; ++i) {
		int tmp = str[i] - '0';
		int idx = n - i - 1;
		if (tmp == 0) {
			// 忽略2005,150中多余的0
			if (str[i - 1] == '0' || i == n - 1) {
				continue;
			}// 万位多余的0，比如205455
			else if (idx >= 4 && idx % 4 == 0) {
				res += Unit[idx];
			}// 正常的0
			else {
				res += Num[tmp];
			}
		}
		else {
			res += Num[tmp];
			if (n == 2 && str[0] == '1' && i == 0) {
				res.erase(0); //15十五，删掉1
			} 
			if (idx % 4 == 0) {
				res += Unit[idx]; // 万位，亿位
			}
			else { 
				res += Unit[idx % 4];
			}
		}
	}

	return res;
}

int main() {
	int x = 0;
	cin >> x;
	cout << NumberToChinese(x) << endl;
}