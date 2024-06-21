#pragma once
#include "../api.h"
#define API_NAME Api1
#define API_CLASS TEST_CASE(Api1)
#define API_INPUT Input_Api1
class Temp : public Input{
    public:
        Temp(){
        }
        int num;
        int abs;
        void parse() override {
            SET_PARAMS(num, true);
            SET_PARAMS(abs, true);
            cout<<num<<endl;
            cout<<abs<<endl;
        }
};
class API_INPUT: public Input{
    public:
        API_INPUT(){
        }
        int sd1=0;
        int sd2=0;
        vector<int> sd3;
        vector<Temp> temps;
        Temp temp;
        void parse() override{
            SET_PARAMS(sd1, true);
            SET_PARAMS(sd2, true);
            SET_PARAMS_CLASS(temp, true);
            SET_PARAMS_LIST(sd3, true);
            SET_PARAMS_LIST_CLASS(temps, true);
        }
};
