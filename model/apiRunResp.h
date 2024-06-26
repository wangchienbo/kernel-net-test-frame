#pragma once
#include <string>
using namespace std;
const int noFindErrorCode = 0;
const int SUCCESSCode = 1;
const int testErrorCode = 2;
const int RequestErrorCode = 3;
const int RunnningErrorCode = 4;
class apiRunResp
{
    public:
        int code;
        string data;
        string msg;
        string json;
};
