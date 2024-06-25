#pragma once
#include "../common/common.h"
#include "../model/model.h"
#include "../store/store.h"

void saveReportService(runTestResp resp, runTestReq req){
    string reportName = resp.reportName;
    string report ="";
    if(resp.code == SUCCESSCode){
        report += "API Name: " + req.apiName + "\n";
        report += "Request Body: " + req.requestBody + "\n";
        report += "Response Code: " + to_string(resp.testResult.responseCode) + "\n";
        report += "Response Result: " + resp.testResult.responseData + "\n";
        report += "统计: \n 1 成功\n, 0 失败\n";
    } else {
        report += "API Name: " + req.apiName + "\n";
        report += "Request Body: " + req.requestBody + "\n";
        report += "Response Code: " + to_string(resp.testResult.responseCode) + "\n";
        report += "失败原因: \n" + resp.testResult.responseData + "\n";
        report += "统计: \n 0 成功\n, 1 失败\n";
    }
    saveReportStore(reportName, report);
    return ;
}