#include <iostream>
#include <mutex>
#include <memory>
#include <gtest/gtest.h>


class Singleton {
private:
    Singleton() {
        std::cout << "Singleton constructor" << std::endl;
    }
    
    static std::unique_ptr<Singleton> instance_ptr; 
    static std::once_flag once_flag_instance;
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;


    static Singleton* getInstance() {
        std::call_once(once_flag_instance, []() {
            instance_ptr.reset(new Singleton());
        });
        return instance_ptr.get();
    }
    
    void showMessage() {
        std::cout << "Singleton showMessage" << std::endl;
    }

}; 

std::unique_ptr<Singleton> Singleton::instance_ptr = nullptr;
std::once_flag Singleton::once_flag_instance;

TEST(SingletonTest, GetInstance) {
    Singleton* instance1 = Singleton::getInstance();
    Singleton* instance2 = Singleton::getInstance();
    instance1->showMessage();
    instance2->showMessage();

    EXPECT_EQ(instance1, instance2);
}