#pragma once
#include "myTest/testApi/allApi.h"
#include "runTest.h"
#include "saveReport.h"
runCaseResp runCaseService(runCaseReq req){
    runCaseResp resp;
    std::map<std::string, std::shared_ptr<TestCase>> test_cases;
    test_cases = TestCaseRegistry::Instance().GetTestCases();
    apiRunResp *apiResp;
    runTestReq req_;
    vector<string> allCase = getTestStore("",req.testCaseName);
    if(allCase.size()==0){
        throw notFoundCaseExpection("Test case not found: " + req.testCaseName);
        return resp;
    }
    req_.json = allCase[0];
    req_.parse();
    req_.needReport = "false";
    runTestResp rtr = runTestService(req_);
    resp.testCaseName = req.testCaseName;
    resp.testResult = rtr.testResult;
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