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
        void setParamsClass(Input& input, string key, bool required) {
            
            string s1= getparams(json,key);
            input.json = s1;
            input.parse();
        }
        template <typename T>
        void setParams(T t, string key, bool required) {
            string s1= getparams(json,key);
            std::stringstream ss;
            ss << s1;
            ss >> *t;
            if (ss.fail()) {
                if(required) throw parseExpection("parse error");
                t=nullptr;
            }
        }
        std::string getparams(string url, const std::string& key);
};
bool areBracketsBalanced(const std::string& str) {
    std::stack<char> brackets;
    for (char ch : str) {
        switch (ch) {
            case '{':
            case '(':
            case '[':
                brackets.push(ch);
                break;
            case '}':
                if (brackets.empty() || brackets.top() != '{') {
                    return false;
                }
                brackets.pop();
                break;
            case ')':
                if (brackets.empty() || brackets.top() != '(') {
                    return false;
                }
                brackets.pop();
                break;
            case ']':
                if (brackets.empty() || brackets.top() != '[') {
                    return false;
                }
                brackets.pop();
                break;
        }
    }

    return brackets.empty();
}
bool findOk(string url,string key_qual, size_t start_pos){
    if(start_pos==0){
        return true;
    }
    if(url[start_pos-1] != '?' && url[start_pos-1] != '&'){
        return false;
    }
    string sub_str = url.substr(0, start_pos);
    if(areBracketsBalanced(sub_str)){
        return true;
    }
    return false;
}
std::string Input::getparams(string url, const std::string& key) {
    if(url.length() > 0 && (url[0] == '{' || url[0] == '[' || url[0] == '(')){
        url = url.substr(1, url.length() - 2);
    }
    std::string key_equal = key + "=";
    size_t start_pos = 0;
    size_t end_pos = 0;
    if((start_pos = url.find(key_equal, start_pos)) == std::string::npos){
        cout << start_pos << endl;
        return "";
    }
    while(!findOk(url,key_equal,start_pos)){
        cout << start_pos << endl;
        start_pos += key_equal.length();
        if ((start_pos = url.find(key_equal, start_pos)) == std::string::npos){
            return "";
        }
    }
    start_pos += key_equal.length();
    end_pos = start_pos;
    while ((end_pos= url.find("&", end_pos)) != std::string::npos) {
        std::string value = url.substr(start_pos, end_pos - start_pos);
        // 检查括号是否成对
        if (areBracketsBalanced(value)) {
            if (value.length() > 0 && (value[0] == '{' || value[0] == '[' || value[0] == '(')){
                value = value.substr(1, value.length() - 2);
            }
            return value;
        }
        // 如果括号不成对，从下一个字符开始继续查找
        end_pos = end_pos + 1;
    }
    std::string value = url.substr(start_pos, url.length() - start_pos);
    if (areBracketsBalanced(value)) {
        if (value.length() > 0 && (value[0] == '{' || value[0] == '[' || value[0] == '(')){
            value = value.substr(1, value.length() - 2);
        }
        return value;
    }
    return "";
}
template <typename T>
T static_cast_input(Input* ip){
    return *(static_cast<T*>(ip));
}