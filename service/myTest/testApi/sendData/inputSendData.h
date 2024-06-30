#pragma once
#include "../api.h"
#define API_NAME SendData
#define API_CLASS TEST_CASE(SendData)
#define API_INPUT Input_SendData
class API_INPUT: public Input{
    public:
        API_INPUT(){
        }
        string targetIp;
        int targetPort;
        int dataSize;
        void parse() override{
            SET_PARAMS(targetIp, true);
            SET_PARAMS(targetPort, true);
            SET_PARAMS(dataSize, true);
        }
};
