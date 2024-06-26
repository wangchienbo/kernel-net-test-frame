#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
using namespace std;

class getTestReq : public apiRunReq {
    public:
        string apiName;
        string testCaseName;
        void parse(){
            SET_PARAMS(apiName ,false);
            SET_PARAMS(testCaseName ,false);
        }
        void unparse(){
            
        }
};
class getTestResp:public apiRunResp{
    public:
        vector<string> data;
        void parse(){
        }
        void unparse(){
            json.clear();
            SET_OUTPUT(msg);
            SET_OUTPUT_LIST(data);
            END_UNPARSE
        }
};