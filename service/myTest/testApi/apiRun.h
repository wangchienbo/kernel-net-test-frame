#include "allApi.h"
#include "api.h"
class ApiRun {
    public:
        apiRunResp *resp;
        std::map<std::string, std::shared_ptr<TestCase>> test_cases;
        ApiRun(){
            // 获取所有注册的测试用例
            test_cases = TestCaseRegistry::Instance().GetTestCases();
            resp = new apiRunResp();
        } 
        apiRunResp run(string apiName, string json){
            if (test_cases.find(apiName) == test_cases.end()) {
                resp->code = noFindErrorCode;
                resp->data = "Test case not found: " + apiName;
            }
            else{
                resp = test_cases[apiName]->run(json);
                
            }
            return *resp;
         }
};