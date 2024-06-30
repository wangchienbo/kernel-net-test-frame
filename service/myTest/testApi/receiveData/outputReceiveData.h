#pragma once
#include "../api.h"
#define API_OUTPUT Output_ReceiveData
class API_OUTPUT: public Output{
    public:
        API_OUTPUT(){
        }
        string targetIp;
        int targetPort;
        string result;
        void unparse() override {
            SET_OUTPUT(targetIp);
            SET_OUTPUT(targetPort);
            SET_OUTPUT(result);
            END_UNPARSE
        }
};
