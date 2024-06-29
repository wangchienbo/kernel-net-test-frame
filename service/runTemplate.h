#pragma once
#include <chrono> 
#include "myTest/testApi/allApi.h"
#include "runCase.h"
#include "saveReport.h"
#include "getTemplate.h"
vector<runCaseResp> runCaseList(vector<string>& reqs,int type){
    vector<runCaseResp> resp;
    for(auto testCaseName : reqs){
        runCaseReq runCaseReq_;
        runCaseReq_.testCaseName = testCaseName;
        runCaseReq_.needReport = "false";
        try {
            if(type==2){
                runCaseResp resp_;
                auto start = std::chrono::high_resolution_clock::now();
                int successNum = 0;
                int failNum = 0;
                for(int i=0;i<1000;i++){
                    try {
                        resp_ = runCaseService(runCaseReq_);
                        if(resp_.testResult.responseCode == SUCCESSCode){
                            successNum++;
                        } else {
                            failNum++;
                        }
                    } catch (const std::exception& e) {
                        failNum++;
                    }
                }
                auto end = std::chrono::high_resolution_clock::now(); // 结束时间
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); // 计算运行时间（毫秒）
                float ips = 1000.0/duration;
                resp_.testResult.responseData += "\n共测试1000次，cost time："+std::to_string(duration)+"，QPS: "+to_string(ips)+"/s"+"其中：\n"; 
                resp_.testResult.responseData += "successNum: " + to_string(successNum) + " failNum: " + to_string(failNum);
                resp.push_back(resp_);
            }else{
                runCaseResp resp_ = runCaseService(runCaseReq_);
                resp.push_back(resp_);
            }
            
        } catch (const std::exception& e) {
            runCaseResp resp_;
            resp_.testCaseName = testCaseName;
            resp_.testResult.responseData = e.what();
            resp_.testResult.responseData+="\n";
            resp_.testResult.responseData+="running "+testCaseName+" error";
            resp_.testResult.responseCode = RunnningErrorCode;
            resp.push_back(resp_);
        }
    }
    return resp;
}
runTemplateResp runTemplateService(runTemplateReq req){
    runTemplateResp resp;
    
    getTemplateReq getTemplateReq_;
    getTemplateReq_.templateName = req.templateName;
    getTemplateResp getTemplateResp_ = getTemplateService(getTemplateReq_);
    resp.singleResults = runCaseList(getTemplateResp_.singleTestCases,1);
    resp.performanceResults = runCaseList(getTemplateResp_.performanceTestCases,2);
    resp.safeTestResults = runCaseList(getTemplateResp_.safeTestCases,3);
    resp.compatibilityResults = runCaseList(getTemplateResp_.compatibilityTestCases,4);
    if(req.needReport=="true"){
        if(isFileNameValid(req.reportName)){
            resp.reportName = req.reportName;
        } else {
            resp.reportName = req.templateName + "_" + getCurrentTime();
        }
        try {
            saveRunTemplateReportService(resp, req, getTemplateResp_);
        }
        catch (const std::exception& e) {
            resp.msg = e.what();
        }
    }
    resp.reportName = req.reportName;
    resp.unparse();
    return resp;
}