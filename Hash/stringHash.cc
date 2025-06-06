#include <iostream>
#include <string>

// 一个经典的、简单的字符串哈希函数实现 (DJB2)
// 特点：实现非常简单，性能较好，分布性也不错。
unsigned long djb2_hash(const char* str) {
    unsigned long hash = 5381; // 这是一个魔数，经验之选
    int c;

    while ((c = *str++)) {
        // hash * 33 + c  等价于 (hash << 5) + hash + c
        // 位运算通常比乘法更快
        hash = ((hash << 5) + hash) + c; 
    }

    return hash;
}


int main() {
    // ---- Part 1: 测试 DJB2 哈希函数 ----
    const char* test_str1 = "hello_world";
    const char* test_str2 = "hello_world!"; // 微小改变
    std::cout << "DJB2 hash for \"" << test_str1 << "\": " << djb2_hash(test_str1) << std::endl;
    std::cout << "DJB2 hash for \"" << test_str2 << "\": " << djb2_hash(test_str2) << std::endl;
    std::cout << "---" << std::endl;

    std::string test_str3 = "hello_world";
    std::string test_str4 = "hello_world!";
    std::cout << "DJB2 hash for \"" << test_str3 << "\": " << djb2_hash(test_str3.c_str()) << std::endl;
    std::cout << "DJB2 hash for \"" << test_str4 << "\": " << djb2_hash(test_str4.c_str()) << std::endl;
    std::cout << "---" << std::endl;

    return 0;
}