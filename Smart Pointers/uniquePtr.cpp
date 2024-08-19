#include <iostream>

template <typename T>
class uniquePtr {
private:
	T* ptr;

public:
	explicit uniquePtr(T *p = nullptr): ptr(p) { }

	uniquePtr(const uniquePtr&) = delete;
	uniquePtr& operator=(const uniquePtr&) = delete;

	uniquePtr(uniquePtr&& other) noexcept : ptr(other.ptr) {
		other.ptr = nullptr;
	}

	uniquePtr& operator=(uniquePtr&& other) noexcept {
		if (this != &other) {
			delete ptr;
			ptr = other.ptr;
			other.ptr = nullptr;
		}

		return *this;
	}

	~uniquePtr() {
		delete ptr;
	}

	T& operator*() const {
		return *ptr;
	}

	T* operator->() const {
		return ptr;
	}

	T* get() const {
		return ptr;
	}

	T* release() {
		T* old = ptr;
		ptr = nullptr;
		return old;
	}

	void reset(T* p = nullptr) {
		if (ptr != p) {
			delete ptr;
			ptr = p;
		}
	}
};

class Test {
public:
	void show() {
		std::cout << "Test::show()" << std::endl;
	}
};

int main() {
	uniquePtr<Test> p(new Test());
	p->show();

	uniquePtr<Test> p2 = std::move(p);
	if (!p.get()) {
		std::cout << "p is empty" << std::endl;
	}

	p2->show();

	return 0;
}