#pragma once
#include <string>
#include <stack>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "http/http.h"
#include "myExpection/myExpection.h"
#include <fstream>
#include <sys/stat.h>

#define SET_PARAMS(key, required) setParams(&key, json, #key, required)
#define SET_PARAMS_CLASS(key, required) setParamsClass(key, json, #key, required)
#define SET_PARAMS_LIST(key, required) setParamsList(key, json, #key, required)
#define SET_PARAMS_LIST_CLASS(key, required) setParamsListClass(key, json, #key, required)
#define SET_OUTPUT(key) setOutput(&key, json,std::string(#key))
#define SET_OUTPUT_CLASS(key) setOutputClass(key, json,  std::string(#key))
#define SET_OUTPUT_LIST(key) setOutputList(key, json, std::string(#key))
#define SET_OUTPUT_LIST_CLASS(key) setOutputListClass(key, json, std::string(#key))
#define END_UNPARSE json+="}";
std::string getparams(string url, const std::string& key);
bool areBracketsBalanced(const std::string& str);
std::pair<bool, int> CheckAndConvert(const std::string& s) {
    for (char c : s) {
        if (!std::isdigit(c)) {
            return {false, 0};
        }
    }
    return {true, std::stoi(s)};
}

template <typename T>
void setParamsListClass(std::vector<T>& input, const std::string& json_, const std::string& key, bool required) {
    std::string s1 = getparams(json_, key);
    std::istringstream ss(s1);
    std::string item;
    std::string temp;
    while (std::getline(ss, temp, ',')) { // 自定义逻辑以确保 {} 和 [] 平衡
        item += temp;
        if (areBracketsBalanced(item)) {
            T value;
            value.json = item;
            value.parse();
            input.push_back(value);
            item.clear(); // 清空 item 以解析下一个
        } else {
            item += ","; // 如果不平衡，添加逗号继续读取
        }
    }
}

template <typename T>
void setParamsList(std::vector<T>& input, const std::string& json_, const std::string& key, bool required) {
    std::string s1 = getparams(json_, key);
    std::cout<<"解析"<<json_<<" 解析key为 "<<key<<" 解析结果为 "<<s1<<std::endl;
    std::istringstream ss(s1);
    std::string item;
    while (std::getline(ss, item, ',')) { // 假设列表项由逗号分隔
        std::stringstream itemStream(item);
        T value;
        itemStream >> value; // 将字符串转换为 T 类型
        input.push_back(value);
    }
    cout<<"解析"<<json_<<" 解析key为 "<<key<<" 解析结果为 "<<s1<<" 长度为 "<<input.size()<<endl;
}

template <typename T>
void setParamsClass(T& input,string json_, string key, bool required) {
    string s1= getparams(json_,key);
    std::cout<<"解析"<<json_<<" 解析key为 "<<key<<" 解析结果为 "<<s1<<std::endl;
    input.json = s1;
    input.parse();
}
std::string removeQuotes(std::string input) {
    size_t start = input.find_first_of("\"");
    size_t end = input.find_last_of("\"");
    if (start == std::string::npos || end == std::string::npos || start == end) {
        return input; // 没有找到双引号或只有一个双引号，返回原字符串
    }
    return input.substr(start + 1, end - start - 1);
}
template <typename T>
void setParams(T t, string json_, string key, bool required) {
    string s1= getparams(json_,key);
    s1=removeQuotes(s1);
    cout<<"解析"<<json_<<" 解析key为 "<<key<<" 解析结果为 "<<s1<<endl;
    std::stringstream ss;
    ss.clear();
    ss << s1;
    ss >> std::ws >> *t;
    if (ss.fail()) {
        if(required) throw parseExpection("parse error");
        t=nullptr;
    }
}
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
    if(url[start_pos-1] != '?' && url[start_pos-1] != ','){
        return false;
    }
    string sub_str = url.substr(0, start_pos);
    if(areBracketsBalanced(sub_str)){
        return true;
    }
    return false;
}
std::string removeWhitespace(const std::string& input) {
    std::string result;
    for (char ch : input) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            result += ch;
        }
    }
    return result;
}
std::string getparams(string url, const std::string& key) {
    url=removeWhitespace(url);
    if(url.length() > 0 && (url[0] == '{' || url[0] == '[' || url[0] == '(')){
        url = url.substr(1, url.length() - 2);
    }
    std::string key_equal = '"'+key+'"' + ":";
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
    while ((end_pos= url.find(",", end_pos)) != std::string::npos) {
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
void setOutputListClass(std::vector<T>& output_, string& json_, const string& key) {
    if(json_.length()>0){
        json_+=",";
    }else{
        json_+="{";
    }
    json_+='"'+key+'"'+":[";
    for(int i=0;i<output_.size();i++){
        if(i>0){
            json_+=",";
        }
        output_[i].unparse();
        json_+=output_[i].json;
    }
    json_+="]";
}
template <typename T>
void setOutputList(std::vector<T>& output_, string& json_, const string& key) {
    if(json_.length()>0){
        json_+=",";
    }else{
        json_+="{";
    }
    json_+='"'+key+'"'+":[";
    for(int i=0;i<output_.size();i++){
        if(i>0){
            json_+=",";
        }
        std::stringstream ss;
        ss << output_[i];
        if (!ss.fail()) {
            json_+=ss.str();
        }
    }
    json_+="]";
}
template <typename T>
void setOutputClass(T& output_, string& json_, const string& key) {
    if(json_.length()>0){
        json_+=",";
    }else{
        json_+="{";
    }
    json_+='"'+key+'"'+":";
    output_.unparse();
    int subLen=output_.json.length();
    json_+=output_.json;
}
template <typename T>
void setOutput(T t, string& json_, const string& key) {
    if(json_.length()>0){
        json_+=",";
    }else{
        json_+="{";
    }
    json_+='"'+key+'"'+":";
    std::stringstream ss;
    if(std::is_pointer<T>::value){
        ss << *t;
    }else{
        ss << t;
    }
    if (!ss.fail()) {
        json_+=ss.str();
    }
}
bool is_directory_exists(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return S_ISDIR(buffer.st_mode);
    } else {
        return false;
    }
}

void create_directory(const std::string& path) {
    size_t pos = 0;
    std::string current_level;
    while ((pos = path.find_first_of('/', pos)) != std::string::npos) {
        current_level = path.substr(0, pos++);
        if (!current_level.empty() && !is_directory_exists(current_level)) {
            mkdir(current_level.c_str(), 0777); // 创建当前级别目录
        }
    }
    if (!is_directory_exists(path)) {
        mkdir(path.c_str(), 0777); // 创建最终目录
    }
}