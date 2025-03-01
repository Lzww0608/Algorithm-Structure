#include <cassert>
#include <iostream>
#include <cstring>
#include "string.h"

// 用于测试的辅助函数
#define TEST_CASE(name) \
    do { \
        std::cout << "Running test case: " << #name << "... "; \
        test_##name(); \
        std::cout << "PASSED" << std::endl; \
    } while (0)

// 测试构造函数
void test_constructors() {
    // 默认构造
    String s1;
    assert(s1.empty());
    assert(s1.size() == 0);
    assert(s1.capacity() == 15);
    assert(s1.c_str()[0] == '\0');

    // C风格字符串构造
    String s2("hello");
    assert(s2.size() == 5);
    assert(s2.capacity() >= 5);
    assert(strcmp(s2.c_str(), "hello") == 0);

    // 二进制安全构造
    char data[] = {'H', 'e', 'l', 'l', 'o', '\0', 'W', 'o', 'r', 'l', 'd'};
    String s3(data, 11);
    assert(s3.size() == 11);
    assert(memcmp(s3.data(), data, 11) == 0);

    // nullptr 检查
    try {
        String s4(nullptr);
        assert(false);  // 不应该到达这里
    } catch (const std::invalid_argument&) {
        // 预期的异常
    }
}

// 测试拷贝操作
void test_copy_operations() {
    String s1("hello");
    
    // 拷贝构造
    String s2(s1);
    assert(strcmp(s2.c_str(), "hello") == 0);
    assert(s1.c_str() != s2.c_str());  // 深拷贝检查
    
    // 拷贝赋值
    String s3;
    s3 = s1;
    assert(strcmp(s3.c_str(), "hello") == 0);
    assert(s1.c_str() != s3.c_str());  // 深拷贝检查
}

// 测试移动操作
void test_move_operations() {
    // 移动构造
    String s1("hello");
    const char* original_data = s1.c_str();
    String s2(std::move(s1));
    
    assert(s2.c_str() == original_data);  // 数据被移动
    assert(s1.empty());                   // s1 被清空
    assert(s1.c_str() == nullptr);       // s1 的指针被置空
    
    // 移动赋值
    String s3;
    s3 = std::move(s2);
    assert(s3.c_str() == original_data);  // 数据被移动
    assert(s2.empty());                   // s2 被清空
    assert(s2.c_str() == nullptr);       // s2 的指针被置空
}

// 测试容量管理
void test_capacity_management() {
    String s;
    assert(s.capacity() == 15);  // 初始容量
    
    s.reserve(20);
    assert(s.capacity() >= 20);
    
    s.append("hello");
    size_t cap = s.capacity();
    s.shrink_to_fit();
    assert(s.capacity() == 15);  // 回到最小容量
}

// 测试 append 操作
void test_append_operations() {
    String s;
    
    // 普通append
    s.append("hello");
    assert(strcmp(s.c_str(), "hello") == 0);
    
    // 触发扩容的append
    s.append(" world");
    assert(strcmp(s.c_str(), "hello world") == 0);
    
    // 二进制数据append
    char binary[] = {'!', '\0', '!'};
    s.append(binary, 3);
    assert(s.size() == 14);
    assert(memcmp(s.data() + 11, binary, 3) == 0);
}

// 测试边界条件
void test_edge_cases() {
    // 空字符串
    String s1("");
    assert(s1.size() == 0);
    assert(s1.c_str()[0] == '\0');
    
    // 大量数据
    std::string long_str(1000, 'x');
    String s2(long_str.c_str());
    assert(s2.size() == 1000);
    assert(memcmp(s2.data(), long_str.c_str(), 1000) == 0);
}

// 测试异常安全性
void test_exception_safety() {
    // 测试空指针
    try {
        String s1(nullptr);
        assert(false);
    } catch (const std::invalid_argument&) {}
    
    try {
        String s2;
        s2.append(nullptr);
        assert(false);
    } catch (const std::invalid_argument&) {}
}

int main() {
    TEST_CASE(constructors);
    TEST_CASE(copy_operations);
    TEST_CASE(move_operations);
    TEST_CASE(capacity_management);
    TEST_CASE(append_operations);
    TEST_CASE(edge_cases);
    TEST_CASE(exception_safety);
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
