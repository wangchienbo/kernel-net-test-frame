#pragma once
#include <sstream>
#include <map>
#include "httpResponse.h"
class HttpRequest {
    private:
        std::string method;
        std::string url;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
        

    public:
        HttpResponse response;
        int fd;
        std::string getMethod() const;
        void parse_request(const std::string& request) {
        }

        std::string generate_response() {
            // Generate the HTTP response based on the method, url, and version
            // This is just a simple example, you should generate a proper HTTP response here
            return "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
        }
        void setParams(std::string method, std::string url, std::string version, std::map<std::string, std::string> headers, std::string body){
            this->method = method;
            this->url = url;
            this->version = version;
            this->headers = headers;
            this->body = body;
        }
        std::string getBody() {
            return body;
        }
        void setResponse(int code,std::string response){
            this->response.code = code;
            this->response.msg = response;
        }
};