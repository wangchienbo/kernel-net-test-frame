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
#include <chrono>
#include <sstream>
#include <iomanip>
#define SET_PARAMS(key, required) setParams(&key, json, #key, required)
#define SET_PARAMS_CLASS(key, required) setParamsClass(key, json, #key, required)
#define SET_PARAMS_LIST(key, required) setParamsList(key, json, #key, required)
#define SET_PARAMS_LIST_CLASS(key, required) setParamsListClass(key, json, #key, required)
#define SET_OUTPUT(key) setOutput(&key, json,std::string(#key))
#define SET_OUTPUT_CLASS(key) setOutputClass(key, json,  std::string(#key))
#define SET_OUTPUT_LIST(key) setOutputList(key, json, std::string(#key))
#define SET_OUTPUT_LIST_CLASS(key) setOutputListClass(key, json, std::string(#key))
#define END_UNPARSE json+="}";
std::string getparams(string url, const std::string& key, bool needRemoveBound);
bool areBracketsBalanced(const std::string& str);
std::pair<bool, int> CheckAndConvert(const std::string& s) {
    for (char c : s) {
        if (!std::isdigit(c)) {
            return {false, 0};
        }
    }
    return {true, std::stoi(s)};
}
std::string removeQuotes(std::string input) {
    size_t start = input.find_first_of("\"");
    size_t end = input.find_last_of("\"");
    if (start == std::string::npos || end == std::string::npos || start == end || start != 0 || end != input.length() - 1){
        return input; // 没有找到双引号或只有一个双引号，返回原字符串
    }
    return input.substr(start + 1, end - start - 1);
}
template <typename T>
void setParamsListClass(std::vector<T>& input, const std::string& json_, const std::string& key, bool required) {
    std::string s1 = getparams(json_, key,true);
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
bool balanceQuotes(const std::string& str) {
    std::stack<char> quotes;
    for (char ch : str) {
        if (ch == '\"') {
            if (quotes.empty() || quotes.top() != '"') {
                quotes.push(ch);
            } else {
                quotes.pop();
            }
        }
    }
    return quotes.empty();
}
template <typename T>
void setParamsList(std::vector<T>& input, const std::string& json_, const std::string& key, bool required) {
    std::string s1 = getparams(json_, key,true);
    std::cout<<"解析"<<json_<<" 解析key为 "<<key<<" 解析结果为 "<<s1<<std::endl;
    std::istringstream ss(s1);
    std::string item;
    std::string temp;
    while (std::getline(ss, temp, ',')) { // 自定义逻辑以确保 "" 平衡
        item += temp;
        if (balanceQuotes(item)) {
            item = removeQuotes(item);
            std::stringstream ss;
            ss << item;
            T value;
            ss >> std::ws >> value;
            if (!ss.fail()) {
                input.push_back(value);
            }
            item.clear(); // 清空 item 以解析下一个
        } else {
            item += ","; // 如果不平衡，添加逗号继续读取
        }
    }
    cout<<"解析"<<json_<<" 解析key为 "<<key<<" 解析结果为 "<<s1<<" 长度为 "<<input.size()<<endl;
}

template <typename T>
void setParamsClass(T& input,string json_, string key, bool required) {
    string s1= getparams(json_,key,true);
    std::cout<<"解析"<<json_<<" 解析key为 "<<key<<" 解析结果为 "<<s1<<std::endl;
    cout<<s1<<endl;
    input.json = s1;
    input.parse();
}

template <typename T>
void setParams(T t, string json_, string key, bool required) {
    string s1= getparams(json_,key,false);
    s1=removeQuotes(s1);
    if(s1==""){
        if(required) throw parseExpection("parse error, key not found, \n key is :\n"+key+"\njson is :\n"+json_);
        return;
    }
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
std::string getparams(string url, const std::string& key, bool needRemoveBound = true) {
    url=removeWhitespace(url);
    cout<<"url: "<<url<<" key: "<<key<<endl;
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
            if (needRemoveBound && value.length() > 0 && (value[0] == '{' || value[0] == '[' || value[0] == '(')){
                value = value.substr(1, value.length() - 2);
            }
            return value;
        }
        // 如果括号不成对，从下一个字符开始继续查找
        end_pos = end_pos + 1;
    }
    std::string value = url.substr(start_pos, url.length() - start_pos);
    if (areBracketsBalanced(value)) {
        if (needRemoveBound && value.length() > 0 && (value[0] == '{' || value[0] == '[' || value[0] == '(')){
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
        bool needQuotes = false;
        if(std::is_same<T, std::string>::value){
            needQuotes = true;
        }

        if (!ss.fail()) {
            if(needQuotes){
                json_+='"';
            }
            json_+=ss.str();
            if(needQuotes){
                json_+='"';
            }
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
    bool needQuotes = false;
    if(std::is_pointer<T>::value){
        ss << *t;
        if(std::is_same<T, std::string*>::value){
            needQuotes = true;
        }
    }else{
        ss << t;
        if(std::is_same<T, std::string>::value){
            needQuotes = true;
        }
    }
    if (!ss.fail()) {
        if(needQuotes){
            json_+='"';
        }
        json_+=ss.str();
        if(needQuotes){
            json_+='"';
        }

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
bool is_file_exists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
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

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", std::localtime(&in_time_t));
    return std::string(buf);
}

bool isFileNameValid(const std::string& fileName) {
    // 文件名不允许的字符
    const std::string invalidChars = "\\/*?\"<>|";

    // 检查文件名是否为空
    if (fileName.empty()) {
        return false;
    }

    // 检查文件名是否包含非法字符
    if (std::any_of(fileName.begin(), fileName.end(), [&](char ch) {
        return invalidChars.find(ch) != std::string::npos;
    })) {
        return false;
    }

    // 文件名合法
    return true;
}
std::vector<std::string> splitString(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> result;
    size_t startPos = 0;
    size_t endPos = 0;
    std::string token;
    while ((endPos = input.find(delimiter, startPos)) != std::string::npos) {
        token = input.substr(startPos, endPos - startPos);
        if (!token.empty()) { // 避免添加空字符串
            result.push_back(token);
        }
        startPos = endPos + delimiter.length(); // 移动到下一个潜在的分割点
    }
    // 添加最后一个片段（如果存在）
    token = input.substr(startPos);
    if (!token.empty()) { // 避免添加空字符串
        result.push_back(token);
    }
    return result;
}


std::string queryStrToJson(const std::string& query) {
    std::stringstream result;
    std::vector<std::string> pairs;
    std::string::size_type lastPos = 0, pos = 0;

    // 分割字符串
    while ((pos = query.find_first_of('&', lastPos)) != std::string::npos) {
        pairs.push_back(query.substr(lastPos, pos - lastPos));
        lastPos = pos + 1;
    }
    pairs.push_back(query.substr(lastPos));

    // 处理每个键值对
    bool first = true;
    result << "{";
    for (const auto& pair : pairs) {
        if (!first) {
            result << ",";
        } else {
            first = false;
        }
        auto eqPos = pair.find('=');
        if (eqPos != std::string::npos) {
            result << "\"" << pair.substr(0, eqPos) << "\":" << pair.substr(eqPos + 1);
        }
    }
    result << "}";
    if (result.str() == "{}") {
        return query;
    }
    return result.str();
}

// std::string jsonToQueryStr(const std::string& json) {
//     std::string result;
//     std::string::size_type lastPos = 0, pos = 0;
//     while ((pos = json.find_first_of(':', lastPos)) != std::string::npos) {
//         std::string key = json.substr(lastPos + 1, pos - lastPos - 2);
//         lastPos = pos + 1;
//         pos = json.find_first_of(',', lastPos);
//         if (pos == std::string::npos) {
//             pos = json.find_first_of('}', lastPos);
//         }
//         std::string value = json.substr(lastPos + 1, pos - lastPos - 2);
//         if (!result.empty()) {
//             result += "&";
//         }
//         result += key + "=" + value;
//         lastPos = pos + 1;
//     }
//     return result;
// }
std::string jsonToQueryStr(const std::string& json) {
    std::string result;
    std::string::size_type lastPos = 0, pos = 0;
    while ((pos = json.find_first_of(':', lastPos)) != std::string::npos) {
        // 调整以去除键的双引号
        std::string key = json.substr(lastPos + 2, pos - lastPos - 3);
        lastPos = pos + 1;
        pos = json.find_first_of(',', lastPos);
        if (pos == std::string::npos) {
            pos = json.find_first_of('}', lastPos);
        }
        // 调整以去除值的双引号
        std::string value = json.substr(lastPos + 1, pos - lastPos - 2);
        if (!result.empty()) {
            result += "&";
        }
        result += key + "=" + value;
        lastPos = pos + 1;
    }
    return result;
}