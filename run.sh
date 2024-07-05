#!/bin/bash

# 检查是否有文件名作为参数传入
if [ "$#" -ne 1 ]; then
    echo "用法: $0 <C++文件>"
    exit 1
fi

# 获取文件名（不含扩展名）用于输出文件
filename=$(basename -- "$1")
filename="${filename%.*}"

# 编译C++文件
g++ -o "$filename" -pthread "$1" -std=c++11

# 检查编译是否成功
if [ $? -eq 0 ]; then
    echo "编译成功，正在执行..."
    ./"$filename"
else
    echo "编译失败."
fi