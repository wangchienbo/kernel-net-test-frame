#pragma once
#include <map>
#include <string>
#include <memory>
#include <mutex>
#include "fileTools/fileTools.h"
#include "math.h"
using namespace std;
class case2apiName {
private:
    std::map<std::string, std::string> dataMap;
    std::mutex mapLock;
    // 私有构造函数
    case2apiName() {
    }

public:
    // 获取单例实例的静态方法
    static case2apiName &get_instance() {
        static case2apiName instance;
        return instance;
    }
    void init() {
        cout << "init case2apiName" << endl;
        if (is_directory_exists("cases") == false) {
            return;
        }
        vector<string> filenames = readDirectoryDirNames("cases");
        for (auto filename : filenames) {
            auto caseNames = readDirectoryFilenames("cases/" + filename);
            for (auto caseName : caseNames) {
                caseName = caseName.substr(0, caseName.find("."));
                cout << "caseName:" << caseName << endl;
                dataMap[caseName] = filename;
            }
        }
    }
    string getdata(string key) {
        std::lock_guard<std::mutex> lock(mapLock);
        if (dataMap.find(key) == dataMap.end()) {
            return "";
        }
        return dataMap[key];
    }
    bool setdata(string key, string value) {
        std::lock_guard<std::mutex> lock(mapLock);
        if (dataMap.find(key) != dataMap.end()) {
            return false;
        }
        dataMap[key] = value;
        return true;
    }
};