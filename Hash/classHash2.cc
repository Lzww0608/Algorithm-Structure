#include <iostream>
#include <string>
#include <unordered_map>
#include <functional> // for std::hash

// 自定义类作为键
class MyKey {
public:
    int id;
    std::string name;

    MyKey(int i, const std::string& n) : id(i), name(n) {}

    // 1. 提供相等比较函数 (operator==)
    bool operator==(const MyKey& other) const {
        return id == other.id && name == other.name;
    }

    // 为了方便打印
    friend std::ostream& operator<<(std::ostream& os, const MyKey& key) {
        os << "ID: " << key.id << ", Name: " << key.name;
        return os;
    }
};

// 2. 提供一个专门的哈希结构体
struct MyKeyHash {
    std::size_t operator()(const MyKey& key) const {
        // 使用 std::hash 组合多个成员的哈希值
        // 一个常见的组合方式是使用异或和移位，或者更健壮的 boost::hash_combine 类似逻辑
        std::size_t h1 = std::hash<int>()(key.id);
        std::size_t h2 = std::hash<std::string>()(key.name);
        return h1 ^ (h2 << 1); // 简单的组合方式，实际应用中可能需要更复杂的组合
    }
};

int main() {
    // 声明 unordered_map 时，需要指定键类型、值类型、哈希函数类型和可选的相等比较函数类型
    // 如果 operator== 在类内定义，则不需要显式指定比较函数类型，它会默认使用 operator==
    std::unordered_map<MyKey, std::string, MyKeyHash> userMap;

    MyKey key1(1, "Alice");
    MyKey key2(2, "Bob");
    MyKey key3(1, "Alice"); // 与 key1 相等

    userMap[key1] = "Data for Alice";
    userMap[key2] = "Data for Bob";

    std::cout << "userMap[key1]: " << userMap[key1] << std::endl;
    std::cout << "userMap[key2]: " << userMap[key2] << std::endl;

    if (userMap.count(key3)) {
        std::cout << "Found key3 (same as key1): " << userMap[key3] << std::endl;
    }

    for (const auto& pair : userMap) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }
    return 0;
}