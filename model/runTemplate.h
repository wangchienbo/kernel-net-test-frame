
#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
#include "runTest.h"
#include "runCase.h"
using namespace std;
// {
//   "templateName": "HighLoadNetworkPerformanceTest",
//   "needReport": true,
//   "reportName": "report1"
// }
class runTemplateReq : public apiRunReq {
public:
    string templateName;
    string needReport;
    string reportName;
    void parse() {
        SET_PARAMS(templateName, true);
        SET_PARAMS(needReport, false);
        SET_PARAMS(reportName, false);
    }
    void unparse() {
        json.clear();
        SET_OUTPUT(templateName);
        SET_OUTPUT(needReport);
        SET_OUTPUT(reportName);
        END_UNPARSE
    }
};
class runTemplateResp : public apiRunResp {
public:
    string templateId;
    string templateName;
    vector<runCaseResp> singleResults;
    vector<runCaseResp> performanceResults;
    vector<runCaseResp> safeTestResults;
    vector<runCaseResp> compatibilityResults;
    string reportName;
    void parse() {
        SET_PARAMS(templateId, true);
        SET_PARAMS(templateName, true);
        SET_PARAMS_LIST_CLASS(singleResults, true);
        SET_PARAMS_LIST_CLASS(performanceResults, true);
        SET_PARAMS_LIST_CLASS(safeTestResults, true);
        SET_PARAMS_LIST_CLASS(compatibilityResults, true);
        SET_PARAMS(reportName, false);
    }
    void unparse() {
        SET_OUTPUT(templateId);
        SET_OUTPUT(templateName);
        SET_OUTPUT_LIST_CLASS(singleResults);
        SET_OUTPUT_LIST_CLASS(performanceResults);
        SET_OUTPUT_LIST_CLASS(safeTestResults);
        SET_OUTPUT_LIST_CLASS(compatibilityResults);
        SET_OUTPUT(reportName);
        SET_OUTPUT(msg);
        END_UNPARSE
    }
};