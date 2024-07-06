#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
using namespace std;
class addTemplateReq : public apiRunReq {
public:
    string templateName;
    vector<string> singleTestCases;
    vector<string> performanceTestCases;
    vector<string> safeTestCases;
    vector<string> compatibilityTestCases;
    string templateDescription;
    string templateId;
    int version_number;
    void parse() {
        SET_PARAMS(templateName, true);
        SET_PARAMS_LIST(singleTestCases, true);
        SET_PARAMS_LIST(performanceTestCases, true);
        SET_PARAMS_LIST(safeTestCases, true);
        SET_PARAMS_LIST(compatibilityTestCases, true);
        SET_PARAMS(templateDescription, true);
        SET_PARAMS(templateId, false);
        SET_PARAMS(version_number, false);
    }
    void unparse() {
        SET_OUTPUT(templateName);
        SET_OUTPUT_LIST(singleTestCases);
        SET_OUTPUT_LIST(performanceTestCases);
        SET_OUTPUT_LIST(safeTestCases);
        SET_OUTPUT_LIST(compatibilityTestCases);
        SET_OUTPUT(templateDescription);
        SET_OUTPUT(templateId);
        SET_OUTPUT(version_number);
        END_UNPARSE
    }
};
class addTemplateResp : public apiRunResp {
public:
    string templateId;
    void parse() {
        SET_PARAMS(msg, true);
    }
    void unparse() {
        SET_OUTPUT(msg);
        SET_OUTPUT(templateId);
        END_UNPARSE
    }
};