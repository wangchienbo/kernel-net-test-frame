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
        string caseId;
        void parse(){
            SET_PARAMS(testCaseName,true);
            SET_PARAMS(apiName,true);
            SET_PARAMS(requestBody,true);
            SET_PARAMS(preconditions,true);
            SET_PARAMS(expectedOutcome,true);
            SET_PARAMS(caseId,false);
        }
        void unparse(){
            SET_OUTPUT(testCaseName);
            SET_OUTPUT(apiName);
            SET_OUTPUT(requestBody);
            SET_OUTPUT(preconditions);
            SET_OUTPUT(expectedOutcome);
            SET_OUTPUT(caseId);
            END_UNPARSE
        }
};
class addCaseResp:public apiRunResp{
    public:
        string caseId;
        void parse(){
            SET_PARAMS(caseId ,true);
        }
        void unparse(){
            SET_OUTPUT(caseId); 
            SET_OUTPUT(msg);
            END_UNPARSE
        }
};