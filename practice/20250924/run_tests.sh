#!/bin/bash

# 检查是否安装了GoogleTest
if ! pkg-config --exists gtest; then
    echo "GoogleTest未安装，请先安装GoogleTest"
    echo "Ubuntu/Debian: sudo apt-get install libgtest-dev cmake"
    echo "CentOS/RHEL: sudo yum install gtest-devel cmake"
    exit 1
fi

# 创建build目录
mkdir -p build
cd build

# 使用CMake构建项目
cmake ..
make

# 运行测试
if [ -f "./lru_test" ]; then
    echo "运行LRU缓存测试用例..."
    ./lru_test
else
    echo "编译失败"
    exit 1
fi
