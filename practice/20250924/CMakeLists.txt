cmake_minimum_required(VERSION 3.14)
project(LRUCacheTest)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找GoogleTest
find_package(GTest REQUIRED)

# 创建测试可执行文件
add_executable(
    lru_test
    test_lru.cpp
)

# 链接GoogleTest库
target_link_libraries(
    lru_test
    GTest::gtest
    GTest::gtest_main
    pthread
)

# 启用测试
enable_testing()

# 添加测试
add_test(
    NAME lru_cache_tests
    COMMAND lru_test
)
