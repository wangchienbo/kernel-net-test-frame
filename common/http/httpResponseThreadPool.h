#pragma once
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../epoll/responseThreadPool.h"
#include "httpRequest.h"
class myResponseThreadPool: public ResponseThreadPool {
    public:
        using Handler = std::function<void(HttpRequest&)>;
        std::string method;
        std::string url;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
        std::unordered_map<std::string, Handler> pathHandlersGet;
        std::unordered_map<std::string, Handler> pathHandlersPost;
        std::unordered_map<std::string, Handler> pathHandlersPut;
        std::unordered_map<std::string, Handler> pathHandlersDelete;
        std::string replaceEscapeSequences(const std::string& input) {
            std::string output;
            for (size_t i = 0; i < input.length(); ++i) {
                if (input[i] == '\\' && i + 1 < input.length()) {
                    // Check the next character after the backslash
                    char nextChar = input[i + 1];
                    switch (nextChar) {
                        case 'r':
                            output.push_back('\r');
                            break;
                        case 'n':
                            output.push_back('\n');
                            break;
                        // Add more cases here for other escape sequences if needed
                        default:
                            output.push_back(input[i]);  // Keep the original backslash
                            continue;  // Skip the increment in the loop
                    }
                    i++;  // Increment index to skip the next character after the backslash
                } else {
                    // For all other characters, just copy them
                    output.push_back(input[i]);
                }
            }
            return output;
        }
        void parse(const std::string& str) {
            cout<<"解析"<<str<<endl;
            std::istringstream sstr(str);
            // Parse the request line
            std::getline(sstr, method, ' ');
            std::getline(sstr, url, ' ');
            std::getline(sstr, version, '\n');

            // Normalize the version by removing "\r"
            version.erase(std::remove(version.begin(), version.end(), '\r'), version.end());
            std::string line;
            while (std::getline(sstr, line, '\n') && line != "\r") {
                auto colon = line.find(':');
                if (colon != std::string::npos) {
                    auto name = line.substr(0, colon);
                    auto pos = colon + 1; 
                    while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) {
                        ++pos; 
                    }
                    auto value = line.substr(pos);
                    value.erase(std::remove(value.begin(), value.end(), '\r'), value.end()); // Remove any '\r'
                    headers[name] = value;
                }
            }

            std::cout << "headers:" << std::endl;
            for (const auto& header : headers) {
                std::cout << header.first << ": " << header.second << std::endl;
            }
            body = std::string(std::istreambuf_iterator<char>(sstr), {});
            std::cout << "body: " << body << std::endl;
        }
        virtual void processResult(pair<int,std::vector<uint8_t>> res){
            // process the result
            std::string strWithEscapeSequences(res.second.begin(), res.second.end());
            std::string str = replaceEscapeSequences(strWithEscapeSequences);
            cout<<"解析"<< str <<endl;
            parse(str);
            HttpRequest req;
            cout<<"解析完毕"<<endl;
            cout << "method: " << method << endl;
            if(method == "GET"){
                cout<<"GET"<<endl;
                cout<<url<<endl;
                cout<<pathHandlersGet.size()<<endl;
                if(pathHandlersGet.find(url) != pathHandlersGet.end()){
                    req.setParams(method, url, version, headers, body);
                    cout<<"处理"<<endl;
                    pathHandlersGet[url](req);
                }
            }else if(method == "POST"){
                if(pathHandlersPost.find(url) != pathHandlersPost.end()){
                    req.setParams(method, url, version, headers, body);
                    pathHandlersPost[url](req);
                }
            }else if(method == "PUT"){
                if(pathHandlersPut.find(url) != pathHandlersPut.end()){
                    req.setParams(method, url, version, headers, body);
                    pathHandlersPut[url](req);
                }
            }else if(method == "DELETE"){
                if(pathHandlersDelete.find(url) != pathHandlersDelete.end()){
                    req.setParams(method, url, version, headers, body);
                    pathHandlersDelete[url](req);
                }
            }
            cout<<"处理完毕"<<endl;
            result.msg = req.response;
        }
        virtual void sendRes(int fd){
            string res="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 11\r\n\r\n";
            res+="\n";
            res+=result.msg+"#";
            cout<<"返回"<<res<<endl;
            send(fd, res.c_str(), res.size(), 0);
        }
};