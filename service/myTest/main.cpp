#include "testApi/allApi.h"
int main() {
    // 获取所有注册的测试用例
    auto test_cases = TestCaseRegistry::Instance().GetTestCases();
    string apiName="Api1";
    cout<<test_cases.size()<<endl;
    cout<<test_cases.begin()->first<<endl;
    if (test_cases.find(apiName) == test_cases.end()) {
        std::cerr << "Test case not found: " << apiName << std::endl;
        return 1;
    }else{
        test_cases[apiName]->run("?sd1=1&temp={num=100&abs=10}&sd2=2");
    }
    return 0;
}