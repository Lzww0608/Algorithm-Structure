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

// 2. 特化 std::hash 模板
namespace std { // 必须在 std 命名空间内
    template <>
    struct hash<MyKey> {
        std::size_t operator()(const MyKey& key) const {
            std::size_t h1 = std::hash<int>()(key.id);
            std::size_t h2 = std::hash<std::string>()(key.name);
            return h1 ^ (h2 << 1); // 或者使用更健壮的组合方式
        }
    };
} // namespace std

int main() {
    // 现在声明 unordered_map 时，不需要显式指定哈希函数类型
    std::unordered_map<MyKey, std::string> userMap;

    MyKey key1(1, "Alice");
    MyKey key2(2, "Bob");

    userMap[key1] = "Data for Alice (std::hash specialization)";
    userMap[key2] = "Data for Bob (std::hash specialization)";

    for (const auto& pair : userMap) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }

    return 0;
}