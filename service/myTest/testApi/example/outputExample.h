#pragma once
#include "../api.h"
#define API_OUTPUT Output_Api1
class Temp2 : public Output{
    public:
        Temp2(){
        }
        int num;
        int abs;
        void unparse() override {
            SET_OUTPUT(num);
            SET_OUTPUT(abs);
            END_UNPARSE
        }
};
class API_OUTPUT: public Output{
    public:
        API_OUTPUT(){
        }
        int sd1=0;
        int sd2=0;
        vector<int> sd3;
        vector<Temp2> temps;
        Temp2 temp;
        void unparse() override {
            SET_OUTPUT(sd1);
            SET_OUTPUT(sd2);
            SET_OUTPUT_CLASS(temp);
            SET_OUTPUT_LIST(sd3);
            SET_OUTPUT_LIST_CLASS(temps);
            END_UNPARSE
        }
};
