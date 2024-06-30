#pragma once
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
            apiResp = new apiRunResp();
            thread t1([&]{
                apiResp=test_cases[req.apiName]->run(req.requestBody);
            });
            t1.detach();

            while(apiResp->isRunning&&isFdOpen(req.fd)){
                cout<<"t1 joinable: "<<t1.joinable()<<endl;
                cout<<isFdOpen(req.fd)<<endl;
                cout<<apiResp->isRunning<<endl;
                sleep(1);
            }
            if(isFdOpen(req.fd))cout<<"执行结束"<<endl;
            cout<<isFdOpen(req.fd)<<endl;
            cout<<"t1 joinable: "<<endl;
            test_cases[req.apiName]->TearDown();
            if(!isFdOpen(req.fd)){
                cout << "fd closed" << endl;
                throw connectExpection("fd closed");
            }else{
                cout << "fd open" << endl;
            }
        } catch (const std::exception& e) {
            apiResp = new apiRunResp();
            apiResp->code = RequestErrorCode;
            apiResp->data = e.what();
        }
    }
    resp.apiName = req.apiName;
    resp.testResult.responseCode = apiResp->code;
    resp.testResult.responseData = apiResp->data;
    cout << "expectedOutCome: " << req.expectedOutcome << endl;
    cout << "response Data: " << resp.testResult.responseData << endl;
    if (req.expectedOutcome == resp.testResult.responseData) {
        resp.isTruthValueMatch = "true";
    } else {
        resp.isTruthValueMatch = "false";
    }
    if(req.needReport=="true"){
        if(!isFileNameValid(req.reportName)){
            resp.reportName = req.reportName;
        } else {
            resp.reportName = req.apiName + "_" + getCurrentTime();
        }
        try {
            saveRunTestReportService(resp, req);
        } catch (reportNameExistExpection& e) {
            resp.testResult.msg = e.what();
        }
    }
    return resp;
}