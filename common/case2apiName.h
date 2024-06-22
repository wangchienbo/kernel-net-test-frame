#include <map>
#include <string>
#include <memory>
#include <mutex>
using namespace std;
class case2apiName {
private:
    std::map<std::string, std::string> dataMap;
    std::mutex mapLock;
    // 私有构造函数
    case2apiName() {}

public:
    // 获取单例实例的静态方法
    static case2apiName& get_instance() {
        static case2apiName instance;
        return instance;
    }
    string getdata(string key){
        std::lock_guard<std::mutex> lock(mapLock);
        if(dataMap.find(key) == dataMap.end()){
            return "";
        }
        return dataMap[key];
    }
    bool setdata(string key, string value){
        std::lock_guard<std::mutex> lock(mapLock);
        if (dataMap.find(key) != dataMap.end()){
            return false;
        }
        dataMap[key] = value;
        return true;
    }
};