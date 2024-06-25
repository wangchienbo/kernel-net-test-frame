#include "myTest/testApi/allApi.h"
#include "saveReport.h"
// class ApiRun {
//     public:
//         apiRunResp *resp;
//         std::map<std::string, std::shared_ptr<TestCase>> test_cases;
//         ApiRun(){
//             // 获取所有注册的测试用例
//             test_cases = TestCaseRegistry::Instance().GetTestCases();
//             resp = new apiRunResp();
//         } 
//         apiRunResp run(string apiName, string json){
//             if (test_cases.find(apiName) == test_cases.end()) {
//                 resp->code = noFindErrorCode;
//                 resp->data = "Test case not found: " + apiName;
//             }
//             else{
//                 resp = test_cases[apiName]->run(json);
                
//             }
//             return *resp;
//          }
// };
runTestResp runTestService(runTestReq req){
    runTestResp resp;
    std::map<std::string, std::shared_ptr<TestCase>> test_cases;
    test_cases = TestCaseRegistry::Instance().GetTestCases();
    apiRunResp *apiResp;
    if (test_cases.find(req.apiName) == test_cases.end()) {
        apiResp = new apiRunResp();
        apiResp->code = noFindErrorCode;
        apiResp->data = "Test case not found: " + req.apiName;
    }
    else{
        try {
            apiResp = test_cases[req.apiName]->run(req.requestBody);
        } catch (const std::exception& e) {
            apiResp = new apiRunResp();
            apiResp->code = RequestErrorCode;
            apiResp->data = e.what();
        }
    }
    if (apiResp->code == SUCCESSCode) {
        resp.code = SUCCESSCode;
        resp.apiName = req.apiName;
        resp.testResult.responseCode = apiResp->code;
        resp.testResult.responseData = apiResp->data;
    } else {
        resp.code = testErrorCode;
    }
    if(req.needReport=="true"){
        if(isFileNameValid(req.reportName)){
            resp.reportName = req.reportName;
        } else {
            resp.reportName = req.apiName + "_" + getCurrentTime();
        }
        saveReportService(resp, req);
    }
    return resp;
}