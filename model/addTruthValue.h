#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
using namespace std;

class addTruthValueReq : public apiRunReq {
public:
    string testCaseName;
    string expectedOutcome;
    void parse() {
        SET_PARAMS(testCaseName, true);
        SET_PARAMS(expectedOutcome, true);
    }
    void unparse() {
        SET_OUTPUT(testCaseName);
        SET_OUTPUT(expectedOutcome);
        END_UNPARSE
    }
};

class addTruthValueResp : public apiRunResp {
public:
    // void parse(){
    //     SET_PARAMS(truthValueId ,true);
    // }
    void unparse() {
        SET_OUTPUT(msg);
        END_UNPARSE
    }
};