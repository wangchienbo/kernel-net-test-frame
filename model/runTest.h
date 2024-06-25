#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
using namespace std;
class runTestReq:public apiRunReq{
    public:
        string apiName;
        string requestBody;
        string needReport;
        string reportName;
        void parse(){
            SET_PARAMS(apiName ,true);
            SET_PARAMS(requestBody ,true);
            SET_PARAMS(needReport ,false);
            SET_PARAMS(reportName ,false);
        }
        void unparse(){
            SET_OUTPUT(apiName);
            SET_OUTPUT(requestBody);
            SET_OUTPUT(needReport);
            SET_OUTPUT(reportName);
            END_UNPARSE
        }         
};
// {
//   "code": 200,
//   "data": {
//       "testCaseId": "TC1001",
//       "apiName": "sendData",
//       "testResult": {
//         "responseCode": 200,
//         "responseData": {
//           "throughput": "12MB/s"
//          }
//        },
//        "reportName":"report1",
//        "reportId": "R1001"
//    }
// }
class runResult:public apiRunResp{
    public:
        int responseCode;
        string responseData;
        void parse(){
            SET_PARAMS(responseCode ,true);
            SET_PARAMS(responseData ,true);
        }
        void unparse(){
            SET_OUTPUT(responseCode);
            SET_OUTPUT(responseData);
            END_UNPARSE
        }
};
class runTestResp:public apiRunResp{
    public:
        string apiName;
        runResult testResult;
        string reportName;
        void parse(){
            SET_PARAMS(apiName ,true);
            SET_PARAMS_CLASS(testResult ,true);
            SET_PARAMS(reportName ,false);
        }
        void unparse(){
            SET_OUTPUT(apiName);
            SET_OUTPUT_CLASS(testResult);
            SET_OUTPUT(reportName);
            SET_OUTPUT(msg);
            END_UNPARSE
        }
};