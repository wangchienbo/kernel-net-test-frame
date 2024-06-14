#pragma once
#include "../../../common/common.h"
#include <iostream>
#include <string>
#include <sstream>
#include <stack>
#include <vector>
using namespace std;
class Input {
    public:
        Input() {}
        string json;
        virtual void parse(){}
        void clear(){
            json="";
        }
};
template <typename T>
T static_cast_input(Input* ip){
    return *(static_cast<T*>(ip));
}