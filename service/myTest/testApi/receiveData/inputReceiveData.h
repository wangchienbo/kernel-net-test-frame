#pragma once
#include "../api.h"
#define API_NAME ReceiveData
#define API_CLASS TEST_CASE(ReceiveData)
#define API_INPUT Input_ReceiveData
class API_INPUT: public Input{
    public:
        API_INPUT(){
        }
        int targetPort;
        int dataSize;
        int listenFd;
        int connFd;
        void parse() override{
            SET_PARAMS(targetPort, true);
            SET_PARAMS(dataSize, true);
        }
};
