#pragma once
#include "../server/head.h"
#include "../http/http.h"
void register_(HttpServer* httpserv){
    httpserv->registerPath("/test", "POST", AddTest);
}
