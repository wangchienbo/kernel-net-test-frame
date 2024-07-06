#pragma once
#include <map>
#include <string>
#include <memory>
#include <mutex>
using namespace std;
class fdStatus {
private:
    std::map<int, bool> dataMap;
    std::mutex mapLock;
    // 私有构造函数
    fdStatus() {
    }

public:
    // 获取单例实例的静态方法
    static fdStatus &get_instance() {
        static fdStatus instance;
        return instance;
    }
    bool getdata(int fd) {
        std::lock_guard<std::mutex> lock(mapLock);
        if (dataMap.find(fd) == dataMap.end()) {
            return false;
        }
        return dataMap[fd];
    }
    void setdata(int fd, bool value) {
        std::lock_guard<std::mutex> lock(mapLock);
        dataMap[fd] = value;
    }
    void closedata(int fd) {
        std::lock_guard<std::mutex> lock(mapLock);
        dataMap.erase(fd);
    }
};