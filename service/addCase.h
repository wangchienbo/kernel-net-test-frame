#pragma once
#include "../model/model.h"
#include "../store/store.h"
int addCaseService(addCaseReq& req){
    addCaseResp resp;
    string requestBody = req.json;
    return addAddCaseStore(req.apiName,req.testCaseName,req.json);
}