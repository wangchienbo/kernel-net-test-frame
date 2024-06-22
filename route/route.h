#pragma once
#include "../common/http/http.h"
#include "../server/server.h"
void register_(HttpServer* httpserv){
    cout << "Registering routes" << endl;
    httpserv->registerPath("/test", "GET", RunTest);
    httpserv->registerPath("/addTemplate", "POST", addTemplate);
    httpserv->registerPath("/addCase", "POST", addCase);
}
