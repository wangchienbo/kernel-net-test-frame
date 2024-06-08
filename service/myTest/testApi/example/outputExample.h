#pragma once
#include "../api.h"
#define API_OUTPUT Output_Api1
#define SET_OUTPUT(key) setOutput(&key,std::string(#key))
#define SET_OUTPUT_CLASS(key) setOutputClass(key,  std::string(#key))
class Temp2 : public Output{
    public:
        Temp2(){
        }
        int num;
        int abs;
        void unparse() override {
            SET_OUTPUT(num);
            SET_OUTPUT(abs);
        }
};
class API_OUTPUT: public Output{
    public:
        API_OUTPUT(){
        }
        int sd1=0;
        int sd2=0;
        Temp2 temp;
        void unparse() override {
            SET_OUTPUT(sd1);
            SET_OUTPUT(sd2);
            SET_OUTPUT_CLASS(temp);
        }
};
