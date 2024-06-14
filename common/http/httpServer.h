#pragma once
#include <sstream>
#include <unordered_map>
#include <functional>
#include "../epoll/epollServer.h"
#include "httpRequest.h"
#include "httpResponseThreadPool.h"
class HttpServer : public epollServer {
    public:
        myResponseThreadPool* responsePool;
        using Handler = std::function<void(HttpRequest&)>;
        HttpServer(int port) : epollServer(port){
            myResponseThreadPool* responsePool_ = new myResponseThreadPool();
            pool = new ThreadPool(responsePool_);
            responsePool = responsePool_;
        }
        void registerPath(const std::string& path,const std::string& method, Handler handler) {
            if(method == "GET")
                responsePool->pathHandlersGet[path] = handler;
            else if(method == "POST")
                responsePool->pathHandlersPost[path] = handler;
            else if(method == "PUT")
                responsePool->pathHandlersPut[path] = handler;
            else if(method == "DELETE")
                responsePool->pathHandlersDelete[path] = handler;
        }
};