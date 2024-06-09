#pragma once
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
class Output {
    public:
        Output() {}
        string resStr;
        virtual void unparse(){}
        void setOutputClass(Output& output_, const string& key) {
            resStr+=key+"={";
            output_.unparse();
            int subLen=output_.resStr.length();
            if(subLen>0&&output_.resStr[subLen-1]=='&'){
                output_.resStr=output_.resStr.substr(0,subLen-1);
            }
            resStr+=output_.resStr;
            resStr+="}&";
        }
        template <typename T>
        void setOutput(T t, string key) {
            resStr+=key+"=";
            std::stringstream ss;
            if(std::is_pointer<T>::value){
                ss << *t;
            }else{
                ss << t;
            }
            if (!ss.fail()) {
                resStr+=ss.str();
            }
            resStr+="&";
        }
};