#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
using namespace std;
class getTemplateReq : public apiRunReq {
    public:
        string templateName;
        void parse(){
            SET_PARAMS(templateName ,true);
        }
        void unparse(){
            json.clear();
            SET_OUTPUT(templateName);
            END_UNPARSE
        }
};
class getTemplateResp:public apiRunResp
{
    public:
        string templateId;
        string templateName;
        vector<string> singleTestCases;
        vector<string> performanceTestCases;
        vector<string> safeTestCases;
        vector<string> compatibilityTestCases;
        int version_number;
        void parse(){
            SET_PARAMS(templateId ,true);
            SET_PARAMS(templateName ,true);
            SET_PARAMS_LIST(singleTestCases ,true);
            SET_PARAMS_LIST(performanceTestCases ,true);
            SET_PARAMS_LIST(safeTestCases ,true);
            SET_PARAMS_LIST(compatibilityTestCases ,true);
            SET_PARAMS(version_number ,true);
        }
        void unparse(){
            json.clear();
            SET_OUTPUT(templateId);
            SET_OUTPUT(templateName);
            SET_OUTPUT_LIST(singleTestCases);
            SET_OUTPUT_LIST(performanceTestCases);
            SET_OUTPUT_LIST(safeTestCases);
            SET_OUTPUT_LIST(compatibilityTestCases);
            SET_OUTPUT(version_number);
            END_UNPARSE
        }
};