#pragma once
#include "../epoll/M_connect.h"
#include <unordered_map>
class httpClient : public myconnect
{
    public:
        unordered_map<string, pair<string, string>> name2interface;
        httpClient(string ip, int port):myconnect(ip, port){
            name2interface["addTemplate"] = make_pair("POST","/addTemplate");
            name2interface["runTest"] = make_pair("GET","/runTest");
            name2interface["addCase"] = make_pair("POST","/addCase");
        }
        string Get(string path, string data, string header){
            string req = "GET " + path + " HTTP/1.1\r\n" + header + "\r\n\r\n";
            mywrite(req);
            string response="";
            myread(response);
            return response;
        }
        string Post(string path, string data, string header){
            string req = "POST " + path + " HTTP/1.1\r\n" + header + "\r\n\r\n" + data;
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
        string runRequest(string name, string data){
            string method = name2interface[name].first;
            string path = name2interface[name].second;
            string header = "Content-Length: " + to_string(data.size());
            if(method == "GET"){
                return Get(path, data, header);
            }
            else if(method == "POST"){
                return Post(path, data, header);
            }
            else if(method == "PUT"){
                return Put(path, data, header);
            }
            else if(method == "DELETE"){
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