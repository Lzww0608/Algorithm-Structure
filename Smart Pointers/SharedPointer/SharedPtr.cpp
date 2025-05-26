#include <iostream>
#include <utility>

class RefCount {
public:
	void AddRef() {
		++count;
	}

	int Release() {
		return --count;
	}

	int GetCount() const {
		return count;
	}

private:
	int count{1};
};

template <typename T>
class SharedPtr {
public:
	// 默认构造函数
	SharedPtr(): ptr(nullptr), refCount(nullptr) {
	}

	// 通过裸指针构造函数，显示构造
	explicit SharedPtr(T* p): ptr(p), refCount(new RefCount()){
	}

	// 拷贝构造函数
	SharedPtr(const SharedPtr<T>& other) {
		ptr = other.ptr;
		refCount = other.refCount;
		if (refCount) {
			refCount->AddRef();
		}
	}

	// 移动构造函数
	SharedPtr(SharedPtr<T>&& other) noexcept {
		ptr = other.ptr;
		refCount = other.refCount;
		other.ptr = nullptr;
		other.refCount = nullptr;
	}

	// 拷贝赋值操作符重载
	SharedPtr<T>& operator=(const SharedPtr<T>& other) {
		if (this != &other) {
			Release();

			ptr = other.ptr;
			refCount = other.refCount;
			if (refCount) {
				refCount->AddRef();
			}
		}

		return *this;
	}

	// 移动赋值操作符重载
	SharedPtr<T>& operator=(SharedPtr&& other) noexcept {
		if (this != &other) {
			Release();

			ptr = other.ptr;
			refCount = other.refCount;
			other.ptr = nullptr;
			other.refCount = nullptr;
		}

		return *this;
	}

	// 解引用操作符
	T& operator*() const {
		return *ptr;
	}

	// 成员访问操作符
	T* operator->() const {
		return ptr;
	}

	// 获取裸指针
	T* get() const {
		return ptr;
	}

	// 获取引用计数
	int use_count() const {
		return refCount? refCount->GetCount() : 0;
	}

	// 析构函数
	~SharedPtr() {
		Release();
	}

private:
	T* ptr;
	RefCount* refCount;

	// 释放资源
	void Release() {
		if (refCount && refCount->Release() == 0) {
			delete ptr;
			delete refCount;
		}
		ptr = nullptr;
		refCount = nullptr;
	}
};

class Test {
public:
	Test() {
		std::cout << "Test created\n";
	}

	~Test() {
		std::cout << "Test destroyed\n";
	}

	void show() {
		std::cout << "Test show\n";
	}
};

int main() {
	{
		SharedPtr<Test> sp1(new Test());
		{
			SharedPtr<Test> sp2 = sp1;
			sp2->show();
			std::cout << "Use count:" << sp2.use_count() << "\n";
		}
		std::cout << "Use count after inner scope: " << sp1.use_count() << "\n";
	}
	std::cout << "End of main\n";

	return 0;
}