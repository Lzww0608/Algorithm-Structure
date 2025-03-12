#include <stdint.h>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <cassert>
#include <gtest/gtest.h>

class String {
public:
    static constexpr size_t s_min_capacity = 15;

    String(): data_(nullptr), size_(0), capacity_(s_min_capacity) {
        data_ = new char[capacity_ + 1];
        data_[0] = '\0';
    }
    String(const char* data) {
        if (data == nullptr) {
            throw std::invalid_argument("Null pointer");
        }
        size_ = std::strlen(data);
        capacity_ = std::max(size_, s_min_capacity);
        data_ = new char[capacity_ + 1];
        std::memcpy(data_, data, size_);
        data_[size_] = '\0';
    }

    String(const char* data, size_t len) {
        if (data == nullptr) {
            throw std::invalid_argument("Null pointer");
        }
        size_ = len;
        capacity_ = std::max(size_, s_min_capacity);
        data_ = new char[capacity_ + 1];
        std::memcpy(data_, data, size_);
        data_[size_] = '\0';
    }

    String(const String& other): size_(other.size_), capacity_(other.capacity_) {
        data_ = new char[capacity_ + 1];
        std::memcpy(data_, other.data_, size_ + 1);
    }

    String(String&& other) noexcept: size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
        other.size_ = 0;
        other.capacity_ = 0;
        other.data_ = nullptr;
    }

    String& operator=(const String& other) {
        if (this != &other) {
            size_ = other.size_;
            capacity_ = other.capacity_;
            char* new_data = new char[capacity_ + 1];
            std::memcpy(new_data, other.data_, size_ + 1);
            delete[] data_;
            data_ = new_data;
        }
        return *this;
    }

    String& operator=(String&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
	
	~String() {
		delete []data_;
	}
    
    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            realloc_data(new_capacity);
        }
    }

    void shrink_to_fit() {
        if (size_ < capacity_) {
            realloc_data(size_);
        }
    }

    String& append(const char* str, size_t str_len) {
        if (str == nullptr) {
            throw std::invalid_argument("Null pointer");
        }
        if (str_len + size_ > capacity_) {
            reserve((str_len + size_) * 2);
        }
        std::memcpy(data_ + size_, str, str_len);
        size_ += str_len;
        data_[size_] = '\0';
        return *this;
    }

    String& append(const char* str) {
        if (str == nullptr) {
            throw std::invalid_argument("Null pointer");
        }
        size_t str_len = std::strlen(str);
        append(str, str_len);
        return *this;
    }

    size_t size() const noexcept {return size_;}
    size_t length() const noexcept {return size_;}
    size_t capacity() const noexcept {return capacity_;}
    bool empty() const noexcept {return size_ == 0;}
    const char* c_str() const noexcept {return data_;}
    const char* data() const noexcept {return data_;}
    char& operator[](size_t pos) {
        if (pos >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[pos];
    }

private:
    char* data_;
    size_t size_;
    size_t capacity_;

    void realloc_data(size_t new_capacity) {
        new_capacity = std::max(new_capacity, s_min_capacity);
        char* new_data = new char[new_capacity + 1];
        if (size_ > 0) {
            std::memcpy(new_data, data_, size_);
        }
        new_data[size_] = '\0';
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
};


TEST(StringTest, DefaultConstructor) {
    String s;
    ASSERT_TRUE(s.empty());
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.capacity(), 15);
    ASSERT_EQ(s.c_str()[0], '\0');
}

TEST(StringTest, CStringConstructor) {
    String s("hello");
    ASSERT_EQ(s.size(), 5);
    ASSERT_GE(s.capacity(), 5);
    ASSERT_STREQ(s.c_str(), "hello"); // 使用 ASSERT_STREQ 比较 C 风格字符串
}

TEST(StringTest, BinarySafeConstructor) {
    char data[] = {'H', 'e', 'l', 'l', 'o', '\0', 'W', 'o', 'r', 'l', 'd'};
    String s(data, 11);
    ASSERT_EQ(s.size(), 11);
    ASSERT_EQ(memcmp(s.data(), data, 11), 0);
}

TEST(StringTest, CopyConstructor) {
    String s1("hello");
    String s2(s1);
    ASSERT_STREQ(s2.c_str(), "hello");
    ASSERT_NE(s1.c_str(), s2.c_str()); // 检查深拷贝
}

TEST(StringTest, CopyAssignment) {
    String s1("hello");
    String s2;
    s2 = s1;
    ASSERT_STREQ(s2.c_str(), "hello");
    ASSERT_NE(s1.c_str(), s2.c_str()); // 检查深拷贝
}

TEST(StringTest, MoveConstructor) {
    String s1("hello");
    const char* original_data = s1.c_str();
    String s2(std::move(s1));

    ASSERT_EQ(s2.c_str(), original_data);  // 数据被移动
    ASSERT_TRUE(s1.empty());                   // s1 被清空
    ASSERT_EQ(s1.data(), nullptr);       // s1 的指针被置空
}

TEST(StringTest, MoveAssignment) {
    String s1("hello");
    const char* original_data = s1.c_str();
    String s2;
    s2 = std::move(s1);

    ASSERT_EQ(s2.c_str(), original_data);  // 数据被移动
    ASSERT_TRUE(s1.empty());                   // s1 被清空
    ASSERT_EQ(s1.data(), nullptr);       // s1 的指针被置空
}

TEST(StringTest, Reserve) {
    String s;
    ASSERT_EQ(s.capacity(), 15);
    s.reserve(20);
    ASSERT_GE(s.capacity(), 20);
}

TEST(StringTest, ShrinkToFit) {
    String s;
    s.append("hello");
    s.reserve(30);
    s.shrink_to_fit();
    ASSERT_EQ(s.capacity(), 15);
}

TEST(StringTest, Append) {
    String s;
    s.append("hello");
    ASSERT_STREQ(s.c_str(), "hello");

    s.append(" world");
    ASSERT_STREQ(s.c_str(), "hello world");

    char binary[] = {'!', '\0', '!'};
    s.append(binary, 3);
    ASSERT_EQ(s.size(), 14);
    ASSERT_EQ(memcmp(s.data() + 11, binary, 3), 0);
}

TEST(StringTest, EmptyString) {
    String s("");
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.c_str()[0], '\0');
}

TEST(StringTest, LongString) {
    std::string long_str(1000, 'x');
    String s(long_str.c_str());
    ASSERT_EQ(s.size(), 1000);
    ASSERT_EQ(memcmp(s.data(), long_str.c_str(), 1000), 0);
}

TEST(StringTest, NullptrConstructorException) {
    ASSERT_THROW(String(nullptr), std::invalid_argument);
}

TEST(StringTest, NullptrAppendException) {
    String s;
    ASSERT_THROW(s.append(nullptr), std::invalid_argument);
}

