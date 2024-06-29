#pragma once
#include "../common/http/http.h"
#include "../server/server.h"
void register_(HttpServer* httpserv){
    cout << "Registering caseName2apiNameMap" << endl;
    case2apiName::get_instance().init();
    cout << "Registering routes" << endl;
    httpserv->registerPath("/addTemplate", "POST", addTemplate);
    httpserv->registerPath("/addCase", "POST", addCase);
    httpserv->registerPath("/getTest", "GET", getTest);
    httpserv->registerPath("/runTest", "POST", runTest);
    httpserv->registerPath("/runCase", "POST", runCase);
    httpserv->registerPath("/getTemplate", "GET", getTemplate);
    httpserv->registerPath("/runTemplate", "POST", runTemplate);
    httpserv->registerPath("/addTruthValue", "POST", addTruthValue);
}
