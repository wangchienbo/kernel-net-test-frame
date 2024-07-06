#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
using namespace std;

class getTruthValueReq : public apiRunReq {
public:
    string testCaseName;
    void parse() {
        SET_PARAMS(testCaseName, true);
    }
    void unparse() {
        json.clear();
        SET_OUTPUT(testCaseName);
        END_UNPARSE
    }
};
class getTruthValueResp : public apiRunResp {
public:
    string testCaseName;
    string expectedOutcome;
    void parse() {
        SET_PARAMS(testCaseName, true);
        SET_PARAMS(expectedOutcome, true);
    }
    void unparse() {
        json.clear();
        SET_OUTPUT(msg);
        SET_OUTPUT(data);
        END_UNPARSE
    }
};
