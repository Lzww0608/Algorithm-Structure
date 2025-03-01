#include <cstddef>
#include <cstring>
#include <algorithm>
#include <stdexcept>
class String {
public:
    static constexpr size_t s_min_capacity = 15;

    String(): size_(0), capacity_(s_min_capacity) {
        data_ = new char[capacity_ + 1];
        data_[0] = '\0';
    }

    String(const char* str) {
        if (str == nullptr) {
            throw std::invalid_argument("Null pointer");
        }
        size_ = std::strlen(str);
        capacity_ = std::max(size_, s_min_capacity);
        data_ = new char[capacity_ + 1];
        std::memcpy(data_, str, size_);
        data_[size_] = '\0';
    }

    String(const char* str, size_t len) {
        if (str == nullptr) {
            throw std::invalid_argument("Null pointer");
        }
        size_ = len;
        capacity_ = std::max(size_, s_min_capacity);
        data_ = new char[capacity_ + 1];
        std::memcpy(data_, str, size_);
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

    ~String() {
        delete[] data_;
    }

    String& operator=(const String& other) {
        if (this != &other) {
            char *new_data = new char[other.capacity_ + 1];
            std::memcpy(new_data, other.data_, other.size_);
            delete[] data_;
            data_ = new_data;
            size_ = other.size_;
            capacity_ = other.capacity_;
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
            other.capacity_ = 0;
        }
        return *this;
    }
    
    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            realloc_data(new_capacity);
        }
    }

    void shrink_to_fit() {
        if (capacity_ > size_) {
            realloc_data(size_);
        }
    }

    String& append(const char* str) {
        if (str == nullptr) {
            throw std::invalid_argument("Null pointer");
        }
        size_t str_len = std::strlen(str);
        if (size_ + str_len > capacity_) {
            reserve((size_ + str_len) * 2);
        }
        std::memcpy(data_ + size_, str, str_len);
        size_ += str_len;
        data_[size_] = '\0';
        return *this;
    }

    String& append(const char* str, size_t str_len) {
        if (str == nullptr) {
            throw std::invalid_argument("Null pointer");
        }
        if (str_len == 0) {
            return *this;
        }

        if (size_ + str_len > capacity_) {
            reserve((size_ + str_len) * 2);
        }
        std::memcpy(data_ + size_, str, str_len);
        size_ += str_len;
        data_[size_] = '\0';
        return *this;
    }

    const char* c_str() const noexcept {return data_;}
    const char* data() const noexcept {return data_;}
    size_t size() const noexcept {return size_;}
    size_t length() const noexcept {return size_;};
    size_t capacity() const noexcept {return capacity_;}
    bool empty() const noexcept {return size_ == 0;}
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
        char* new_data = new char[new_capacity];
        if(size_ > 0) {
            std::memcpy(new_data, data_, size_);
        }
        new_data[size_] = '\0';
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
};