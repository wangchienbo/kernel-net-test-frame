#pragma once
#include "../epoll/M_connect.h"
#include "../../model/model.h"
#include "../../common/myExpection/myExpection.h"
#include <unordered_map>
class httpClient : public myconnect
{
    public:
        unordered_map<string, pair<string, string>> name2interface;
        httpClient(string ip, int port):myconnect(ip, port){
            name2interface["addTemplate"] = make_pair("POST","/addTemplate");
            name2interface["runTest"] = make_pair("POST","/runTest");
            name2interface["addCase"] = make_pair("POST","/addCase");
            name2interface["getTest"] = make_pair("GET","/getTest");
            name2interface["runCase"] = make_pair("POST","/runCase");
            name2interface["getTemplate"] = make_pair("GET","/getTemplate");
            name2interface["runTemplate"] = make_pair("POST","/runTemplate");
            name2interface["getTruthValue"] = make_pair("GET","/getTruthValue");
            name2interface["addTruthValue"] = make_pair("POST","/addTruthValue");
        }
        string Get(string path, string data, string header){
            string req = "GET " + path + " HTTP/1.1\r\n" + header + "\r\n\r\n";
            cout << "req:" << req << endl;
            mywrite(req);
            string response="";
            myread(response);
            return response;
        }
        string Post(string path, string data, string header){
            string req = "POST " + path + " HTTP/1.1\r\n" + header + "\r\n\r\n" + data;
            cout << "req:" << req << endl;
            mywrite(req);
            string response="";
            myread(response);
            return response;
        }
        string Put(string path, string data, string header){
            string req = "PUT " + path + " HTTP/1.1\r\n" + header + "\r\n\r\n" + data;
            mywrite(req);
            string response="";
            myread(response);
            return response;
        }
        string Delete(string path, string data, string header){
            string req = "DELETE " + path + " HTTP/1.1\r\n" + header + "\r\n\r\n" + data;
            mywrite(req);
            string response="";
            myread(response);
            return response;
        }
        string parse(string name, string data){
            if(name=="addTemplate"){
                addTemplateReq req;
                req.json=data;
                req.parse();
                req.json ="";
                req.unparse();
                return req.json;
            }
            else if(name=="runTest"){
                runTestReq req;
                req.json=data;
                req.parse();
                req.json ="";
                req.unparse();
                return req.json;
            }
            else if(name=="addCase"){
                addCaseReq req;
                req.json=data;
                req.parse();
                req.json ="";
                req.unparse();
                return req.json;
            }
            else if(name=="getTest"){
                getTestReq req;
                req.json=data;
                req.parse();
                req.json ="";
                req.unparse();
                return req.json;
            }
            else if(name=="runCase"){
                runCaseReq req;
                req.json=data;
                req.parse();
                req.json ="";
                req.unparse();
                return req.json;
            }
            else if(name=="getTemplate"){
                getTemplateReq req;
                req.json=data;
                req.parse();
                req.json ="";
                req.unparse();
                return req.json;
            }
            else if(name=="runTemplate"){
                runTemplateReq req;
                req.json=data;
                req.parse();
                req.json ="";
                req.unparse();
                return req.json;
            }
            else if(name=="getTruthValue"){
                getTruthValueReq req;
                req.json=data;
                req.parse();
                req.json ="";
                req.unparse();
                return req.json;
            }
            else if(name=="addTruthValue"){
                addTruthValueReq req;
                req.json=data;
                req.parse();
                req.json ="";
                req.unparse();
                return req.json;
            }
            return "";
        }
        string runRequest(string name, string data){
            string method = name2interface[name].first;
            string path = name2interface[name].second;
            if (method == "GET" || method == "DELETE"){
                data = queryStrToJson(data);
            }
            cout << "data:" << data << endl;
            data=parse(name, data);
            string header = "Content-Length: " + to_string(data.size());
            if(method == "GET"){
                cout << "data:" << data << endl;
                data = jsonToQueryStr(data);
                cout << "data:" << data << endl;
                path += "?" + data;
                cout << "path:" << path << endl;
                return Get(path, data, header);
            }
            else if(method == "POST"){
                return Post(path, data, header);
            }
            else if(method == "PUT"){
                return Put(path, data, header);
            }
            else if(method == "DELETE"){
                data = jsonToQueryStr(data);
                path += "?" + data;
                return Delete(path, data, header);
            }
            return Get(path, data, header);
        }
        string getAllInterface(){
            string res = "";
            for(auto it = name2interface.begin(); it != name2interface.end(); it++){
                res += it->first + " " + it->second.first + " " + it->second.second + "\n";
            }
            return res;
        }
};