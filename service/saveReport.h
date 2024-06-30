#pragma once
#include "../common/common.h"
#include "../model/model.h"
#include "../store/store.h"

void saveRunTestReportService(runTestResp resp, runTestReq req){
    string reportName = resp.reportName;
    string report ="";
    if(resp.testResult.responseCode == SUCCESSCode){
        report += "API Name: " + req.apiName + "\n";
        report += "Request Body: " + req.requestBody + "\n";
        report += "Response Code: " + to_string(resp.testResult.responseCode) + "\n";
        report += "Response Result: " + resp.testResult.responseData + "\n";
        report += "isPass: " + resp.isTruthValueMatch + "\n";
        report += "统计: \n 1 成功\n 0 失败\n";
    } else {
        report += "API Name: " + req.apiName + "\n";
        report += "Request Body: " + req.requestBody + "\n";
        report += "Response Code: " + to_string(resp.testResult.responseCode) + "\n";
        report += "失败原因: \n" + resp.testResult.responseData + "\n";
        report += "isPass: false\n";
        report += "统计: \n 0 成功\n 1 失败\n";
    }
    saveReportStore(reportName, report);
    return ;
}
void saveRunCaseReportService(runCaseResp resp, runCaseReq req, runTestReq req_){
    string reportName = resp.reportName;
    string report ="";
    if(resp.testResult.responseCode == SUCCESSCode){
        report += "Case Name: " + req.testCaseName + "\n";
        report += "Response Code: " + to_string(resp.testResult.responseCode) + "\n";
        report += "Response Result: " + resp.testResult.responseData + "\n";
        report += "isPass: " + resp.isTruthValueMatch + "\n";
        report += "统计: \n 1 成功\n 0 失败\n";
    } else {
        report += "Case Name: " + req.testCaseName + "\n";
        report += "Request Body: " + req_.json + "\n";
        report += "Response Code: " + to_string(resp.testResult.responseCode) + "\n";
        report += "失败原因: \n" + resp.testResult.responseData + "\n";
        report += "isPass: false\n";
        report += "统计: \n 0 成功\n 1 失败\n";
    }
    saveReportStore(reportName, report);
    return ;
}
void saveRunTemplateReportService(runTemplateResp resp, runTemplateReq req, getTemplateResp req_){
    string reportName = resp.reportName;
    string report ="";
    int successNum = 0;
    int failNum = 0;
    report += "############################################  模版信息  ########################################################\n";
    report += "Template Name: " + req.templateName + "\n";
    report += "templateId: " + req_.templateId + "\n";
    report += "version: " + std::to_string(req_.version_number) + "\n";  
    report += "\n";
    report += "###########################################  单测开始执行  #######################################################\n";
    report += "single Test num: " + std::to_string(req_.singleTestCases.size()) + "\n";
    report += "\n";
    int singleIndex=0;
    int passNum = 0;
    int unpassNum = 0;
    
    for(auto singleCase : resp.singleResults){
        report += "正在测试第" + std::to_string(singleIndex + 1) + "个单测\n";
        report += "single Test Case Name: " + req_.singleTestCases[singleIndex] + "\n";
        report += "Response Code: " + to_string(singleCase.testResult.responseCode) + "\n";
        report += "Response Result: " + singleCase.testResult.responseData + "\n";

        if(singleCase.testResult.responseCode == SUCCESSCode){
            successNum++;
            if (singleCase.isTruthValueMatch == "true") {
                passNum++;
                report += "isPass: true\n";
            } else {
                unpassNum++;
                report += "isPass: false\n";
            }
        } else {
            failNum++;
        }
        singleIndex++;
        report += "\n";
    }
    report += "统计: \n" + std::to_string(passNum) + " 对拍成功. " + std::to_string(unpassNum) + " 对拍失败 \n";
    report += "performance Test num: " + std::to_string(req_.performanceTestCases.size()) + "\n";
    report += "\n";
    report += "#########################################  性能测试开始执行  #####################################################\n";
    int performanceIndex=0;
    for(auto performanceCase : resp.performanceResults){
        report += "正在测试第" + std::to_string(performanceIndex + 1) + "个性能测试\n";
        report += "performance Test Case Name: " + req_.performanceTestCases[performanceIndex] + "\n";
        report += "Response Code: " + to_string(performanceCase.testResult.responseCode) + "\n";
        report += "Response Result: " + performanceCase.testResult.responseData + "\n";
        if(performanceCase.testResult.responseCode == SUCCESSCode){
            successNum++;
        } else {
            failNum++;
        }
        performanceIndex++;
        report += "\n";
    }

    report += "\n";
    report += "#########################################  安全测试开始执行  #####################################################\n";
    report += "safe Test num: " + std::to_string(req_.safeTestCases.size()) + "\n";
    report += "\n";
    int safeIndex=0;
    for(auto safeCase : resp.safeTestResults){
        report += "正在测试第" + std::to_string(safeIndex + 1) + "个安全测试\n";
        report += "safe Test Case Name: " + req_.safeTestCases[safeIndex] + "\n";
        report += "Response Code: " + to_string(safeCase.testResult.responseCode) + "\n";
        report += "Response Result: " + safeCase.testResult.responseData + "\n";
        if(safeCase.testResult.responseCode == SUCCESSCode){
            successNum++;
        } else {
            failNum++;
        }
        safeIndex++;
        report += "\n";
    }
    report += "\n";
    report += "########################################  兼容性测试开始执行  ####################################################\n";
    report += "compatibility Test num: " + std::to_string(req_.compatibilityTestCases.size()) + "\n";
    report += "\n";
    int compatibilityIndex=0;
    for(auto compatibilityCase : resp.compatibilityResults){
        report += "正在测试第" + std::to_string(compatibilityIndex + 1) + "个兼容性测试\n";
        report += "compatibility Test Case Name: " + req_.compatibilityTestCases[compatibilityIndex] + "\n";
        report += "Response Code: " + to_string(compatibilityCase.testResult.responseCode) + "\n";
        report += "Response Result: " + compatibilityCase.testResult.responseData + "\n";
        if(compatibilityCase.testResult.responseCode == SUCCESSCode){
            successNum++;
        } else {
            failNum++;
        }
        compatibilityIndex++;
        report += "\n";
    }
    report += "#############################################  结果总计  #########################################################\n";
    report += "统计:  " + std::to_string(successNum) + " 成功\t" + std::to_string(failNum) + " 失败\n";
    saveReportStore(reportName, report);
    return ;
}