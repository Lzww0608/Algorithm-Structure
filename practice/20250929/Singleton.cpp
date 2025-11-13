

class Singleton {
private:
    Singleton() {}
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }
}


class EagerSingleton {
private:
    EagerSingleton() {}

    static EagerSingleton instance;
public:
    EagerSingleton(const EagerSingleton&) = delete;
    EagerSingleton& operator=(const EagerSingleton&) = delete;

    static EagerSingleton& getInstance() {
        return instance;
    }
}

EagerSingleton EagerSingleton::instance;
