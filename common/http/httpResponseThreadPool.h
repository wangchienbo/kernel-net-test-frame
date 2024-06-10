#include <map>
#include "../epoll/responseThreadPool.h"
#include "httpRequest.h"
class myResponseThreadPool: public ResponseThreadPool {
    public:
        using Handler = std::function<void(const HttpRequest&)>;
        std::string method;
        std::string url;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
        std::unordered_map<std::string, Handler> pathHandlersGet;
        std::unordered_map<std::string, Handler> pathHandlersPost;
        std::unordered_map<std::string, Handler> pathHandlersPut;
        std::unordered_map<std::string, Handler> pathHandlersDelete;

        void parse(const std::string& str) {
            std::istringstream sstr(str);
            sstr >> method >> url >> version;
            std::string line;
            while (std::getline(sstr, line) && line != "\r") {
                auto colon = line.find(':');
                if (colon != std::string::npos) {
                    auto name = line.substr(0, colon);
                    auto value = line.substr(colon + 1);
                    headers[name] = value;
                }
            }
            body = std::string(std::istreambuf_iterator<char>(sstr), {});
        }
        virtual void processResult(pair<int,std::vector<uint8_t>> res){
            // process the result
            std::string str((res.second).begin(),(res.second).end());
            parse(str);
            HttpRequest req;
            if(method == "GET"){
                if(pathHandlersGet.find(url) != pathHandlersGet.end()){
                    req.setParams(method, url, version, headers, body);
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