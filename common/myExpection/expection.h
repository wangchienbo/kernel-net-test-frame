#pragma once
#include <string>
#include <exception>
using namespace std;
// 定义宏，用于声明和注册测试用例
#define myExpection(name)                             \
    class name : public std::exception {              \
    private:                                          \
        std::string message;                          \
                                                      \
    public:                                           \
        name(const std::string &msg) : message(msg) { \
        }                                             \
        const char *what() const noexcept override {  \
            return message.c_str();                   \
        }                                             \
    };
