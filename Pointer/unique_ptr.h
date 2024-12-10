

template <typename T>
class unique_ptr {
private:
    T* ptr;

public:
    // 构造函数
    unique_ptr() noexcept : ptr(nullptr) {}
    
    explicit unique_ptr(T* p) noexcept : ptr(p) {}
    
    // 析构函数
    ~unique_ptr() {
        if(ptr) {
            delete ptr;
            ptr = nullptr;
        }
    }
    
    // 移动构造函数
    unique_ptr(unique_ptr&& other) noexcept {
        ptr = other.ptr;
        other.ptr = nullptr;
    }
    
    // 移动赋值运算符
    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if(this != &other) {
            if(ptr) delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
    
    // 禁用拷贝构造和拷贝赋值
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    
    // 运算符重载
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }
    
    // 释放所有权
    T* release() noexcept {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }
    
    // 重置指针
    void reset(T* p = nullptr) noexcept {
        if(ptr) delete ptr;
        ptr = p;
    }
    
    // 交换
    void swap(unique_ptr& other) noexcept {
        T* temp = ptr;
        ptr = other.ptr;
        other.ptr = temp;
    }
    
    // 布尔转换
    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }
};
