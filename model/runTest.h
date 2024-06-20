#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
using namespace std;
class runTestReq : public apiRunReq {
public:
    string apiName;
    string input;
    void parse() {
        SET_PARAMS(input, true);
        SET_PARAMS(apiName, true);
    }
    void unparse() {
        SET_OUTPUT(input);
        SET_OUTPUT(apiName);
    }
};
class runTestResp : public apiRunResp {
public:
    int code;
    string data;
    void parse() {
        SET_PARAMS(code, true);
        SET_PARAMS(data, true);
    }
    void unparse() {
        SET_OUTPUT(code);
        SET_OUTPUT(data);
        END_UNPARSE
    }
};