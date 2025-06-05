#include <iostream>
#include <gtest/gtest.h>

class Singleton {
private:
    Singleton() {
        std::cout << "Singleton constructor" << std::endl;
    }
    
    static Singleton instance; 
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;


    static Singleton& getInstance() {
        return instance;
    }
    
    void showMessage() {
        std::cout << "Singleton showMessage" << std::endl;
    }
}; 

Singleton Singleton::instance;

TEST(SingletonTest, GetInstance) {
    Singleton& instance1 = Singleton::getInstance();
    Singleton& instance2 = Singleton::getInstance();
    instance1.showMessage();
    instance2.showMessage();

    EXPECT_EQ(&instance1, &instance2);
}