#pragma once
#include "../model/model.h"
#include "../store/store.h"
addCaseResp addCaseService(addCaseReq& req){
    addCaseResp resp;
    req.caseId= req.apiName + "_" + req.testCaseName+ "_" + getCurrentTime();
    req.json.clear();
    req.unparse();
    string requestBody = req.json;
    addAddCaseStore(req.apiName,req.testCaseName,req.json);
    resp.caseId = req.caseId;
    return resp;
}