#pragma once
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../epoll/responseThreadPool.h"
#include "httpRequest.h"
#include "httpResponse.h"
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

            return result.str();
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
            if(method == "GET" || method == "DELETE"){
                body = url.find("?") == std::string::npos ? "" : url.substr(url.find("?") + 1);
                url = url.substr(0, url.find("?"));
                body = queryStrToJson(body);
                std::cout << "body11: " << body<<" dsd "<< this->body<< std::endl;
                return;
            }
            body = std::string(std::istreambuf_iterator<char>(sstr), {});
            std::cout << "body: " << body << std::endl;
        }
        Response processResult(pair<int,std::vector<uint8_t>> res){
            HttpResponse result;
            // process the result
            cout<<res.second.size()<<endl;
            if(res.second.size() == 0){
                cout<<"请求为空"<<endl;
                result.code = 404;
                result.msg = "request is empty";
                return result;
            }
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
                }else{
                    req.setResponse(404,"not found GET："+url);
                }
            }else if(method == "POST"){
                if(pathHandlersPost.find(url) != pathHandlersPost.end()){
                    req.setParams(method, url, version, headers, body);
                    pathHandlersPost[url](req);
                }else{
                    req.setResponse(404,"not found POST："+url);
                }
            }else if(method == "PUT"){
                if(pathHandlersPut.find(url) != pathHandlersPut.end()){
                    req.setParams(method, url, version, headers, body);
                    pathHandlersPut[url](req);
                }else{
                    req.setResponse(404,"not found PUT："+url);
                }
            }else if(method == "DELETE"){
                if(pathHandlersDelete.find(url) != pathHandlersDelete.end()){
                    req.setParams(method, url, version, headers, body);
                    pathHandlersDelete[url](req);
                }else{
                    req.setResponse(404,"not found DELETE："+url);
                }
            }
            cout<<"处理完毕"<<endl;
            result.msg = req.response.msg;
            result.code = req.response.code;
            return result;
        }
        virtual void sendRes(int fd, Response resp){
            HttpResponse *result = static_cast<HttpResponse*>(&resp);
            cout<<"发送"<<result->msg<<endl;
            string res;
            if(result->code==200){
                 res="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "+std::to_string(result->msg.size())+"\r\n\r\n";
            }else{
                 res="HTTP/1.1 "+ std::to_string(result->code) +" OK\r\nContent-Type: text/html\r\nContent-Length: "+std::to_string(result->msg.size())+"\r\n\r\n";
            }
            res+="\n";
            res+=result->msg+"#";
            cout<<"返回"<<res<<endl;
            send(fd, res.c_str(), res.size(), 0);
        }
};