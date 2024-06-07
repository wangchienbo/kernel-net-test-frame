#pragma once
#include "../api.h"
#define API_NAME Api1
#define API_CLASS TEST_CASE(Api1)
#define API_INPUT Input_Api1
#define SET_PARAMS(key, required) setParams(&key, #key, required)
#define SET_PARAMS_CLASS(key, required) setParamsClass(key, #key, required)
class Temp : public Input{
    public:
        Temp(){
        }
        int num;
        int abs;
        void parse() override {
            SET_PARAMS(num, true);
            SET_PARAMS(abs, true);
        }
};
class API_INPUT: public Input{
    public:
        API_INPUT(){
        }
        int sd1=0;
        int sd2=0;
        Temp temp;
        void parse() override{
            SET_PARAMS(sd1, true);
            SET_PARAMS(sd2, true);
            SET_PARAMS_CLASS(temp, true);
        }
};
