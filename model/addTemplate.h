#pragma once
#include <string>
#include "../common/math.h"
#include "apiRunReq.h"
#include "apiRunResp.h"
using namespace std;
class addTemplateReq : public apiRunReq {
public:
    string templateName;
    string testType;
    string testCase;
    void parse() {
        SET_PARAMS(templateName, true);
        SET_PARAMS(testType, true);
        SET_PARAMS(testCase, true);
    }
    void unparse() {
        SET_OUTPUT(templateName);
        SET_OUTPUT(testType);
        SET_OUTPUT(testCase);
    }
};
class addTemplateResp : public apiRunResp {
public:
    string data;
    void parse() {
        SET_PARAMS(data, true);
    }
    void unparse() {
        SET_OUTPUT(data);
        END_UNPARSE
    }
};