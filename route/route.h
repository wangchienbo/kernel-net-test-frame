#pragma once
#include "../common/http/http.h"
#include "../server/server.h"
void register_(HttpServer* httpserv){
    cout << "Registering routes" << endl;
    httpserv->registerPath("/test", "GET", RunTest);
}
