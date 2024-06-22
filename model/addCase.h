#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
using namespace std;
const int ExistErrorCode = 3;
class addCaseReq:public apiRunReq{
    public:
        string testCaseName;
        string apiName;
        string requestBody;
        string preconditions;
        string expectedOutcome;
        void parse(){
            SET_PARAMS(testCaseName,true);
            SET_PARAMS(apiName,true);
            SET_PARAMS(requestBody,true);
            SET_PARAMS(preconditions,true);
            SET_PARAMS(expectedOutcome,true);
        }
        void unparse(){
            SET_OUTPUT(testCaseName);
            SET_OUTPUT(apiName);
            SET_OUTPUT(requestBody);
            SET_OUTPUT(preconditions);
            SET_OUTPUT(expectedOutcome);
            END_UNPARSE
        }
};
class addCaseResp:public apiRunResp{
    public:
        string data;
        void parse(){
            SET_PARAMS(data ,true);
        }
        void unparse(){
            SET_OUTPUT(data);
            END_UNPARSE
        }
};