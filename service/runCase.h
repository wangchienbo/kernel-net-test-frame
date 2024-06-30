#pragma once
#include "myTest/testApi/allApi.h"
#include "runTest.h"
#include "saveReport.h"
#include "getTruthValue.h"
runCaseResp runCaseService(runCaseReq req){
    runCaseResp resp;
    std::map<std::string, std::shared_ptr<TestCase>> test_cases;
    test_cases = TestCaseRegistry::Instance().GetTestCases();
    apiRunResp *apiResp;
    runTestReq req_;
    vector<string> allCase = getTestStore("",req.testCaseName);
    cout << "allCase size: " << allCase.size() << endl;
    if(allCase.size()==0){
        throw notFoundCaseExpection("Test case not found: " + req.testCaseName);
        return resp;
    }
    cout<<"获取真值"<<endl;
    // 获取真值
    getTruthValueReq getTruthValueReq_;
    getTruthValueReq_.testCaseName = req.testCaseName;
    auto getTruthValueResp_ = getTruthValueService(getTruthValueReq_);
    cout<<"真值获取成功"<<endl;

    req_.json = allCase[0];
    req_.parse();
    req_.needReport = "false";
    req_.expectedOutcome = getTruthValueResp_.expectedOutcome;
    cout << "expectedOutCome: " << req_.expectedOutcome << endl;
    req_.fd = req.fd;
    runTestResp rtr = runTestService(req_);
    resp.testCaseName = req.testCaseName;
    resp.testResult = rtr.testResult;
    resp.isTruthValueMatch = rtr.isTruthValueMatch;
    
    // 生成报告
    if(req.needReport=="true"){
        if(isFileNameValid(req.reportName)){
            resp.reportName = req.reportName;
        } else {
            resp.reportName = req.testCaseName + "_" + getCurrentTime();
        }
        try {
            saveRunCaseReportService(resp, req, req_);
        } catch (reportNameExistExpection& e) {
            resp.msg = e.what();
        }
    }
    return resp;
}