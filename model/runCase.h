


#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
#include "runTest.h"
using namespace std;
// {
//   "testCaseName": "TC1001",
//   "needReport": true,
//   "reportName": "report1"
// }
class runCaseReq:public apiRunReq{
    public:
        string testCaseName;
        string needReport;
        string reportName;
        void parse(){
            SET_PARAMS(testCaseName ,true);
            SET_PARAMS(needReport ,false);
            SET_PARAMS(reportName ,false);
        }
        void unparse(){
            json.clear();
            SET_OUTPUT(testCaseName);
            SET_OUTPUT(needReport);
            SET_OUTPUT(reportName);
            END_UNPARSE
        }         
};
class runCaseResp:public apiRunResp{
    public:
        string testCaseName;
        runResult testResult;
        string reportName;
        void parse(){
            SET_PARAMS(testCaseName ,true);
            SET_PARAMS_CLASS(testResult ,true);
            SET_PARAMS(reportName ,false);
        }
        void unparse(){
            SET_OUTPUT(testCaseName);
            SET_OUTPUT_CLASS(testResult);
            SET_OUTPUT(reportName);
            SET_OUTPUT(msg);
            END_UNPARSE
        }
};
