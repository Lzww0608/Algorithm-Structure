#include <iostream>
#include <mutex>
#include <gtest/gtest.h>


class Singleton {
private:
    Singleton() {
        std::cout << "Singleton constructor" << std::endl;
    }
    
    static Singleton* instance; 
    static std::mutex mtx;
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;


    static Singleton* getInstance() {
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mtx);
            if (instance == nullptr) {
                instance = new Singleton();
            }
        }
        return instance;
    }
    
    void showMessage() {
        std::cout << "Singleton showMessage" << std::endl;
    }

    static void destroyInstance() {
        std::lock_guard<std::mutex> lock(mtx);
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }
}; 

Singleton* Singleton::instance = nullptr;
std::mutex Singleton::mtx;

TEST(SingletonTest, GetInstance) {
    Singleton* instance1 = Singleton::getInstance();
    Singleton* instance2 = Singleton::getInstance();
    instance1->showMessage();
    instance2->showMessage();

    EXPECT_EQ(instance1, instance2);

    Singleton::destroyInstance();
}