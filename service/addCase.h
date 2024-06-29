#pragma once
#include "../model/model.h"
#include "../store/store.h"
#include "addTruthValue.h"
addCaseResp addCaseService(addCaseReq& req){
    addCaseResp resp;
    req.caseId= req.apiName + "_" + req.testCaseName+ "_" + getCurrentTime();
    req.json.clear();
    req.unparse();
    string requestBody = req.json;
    addAddCaseStore(req.apiName,req.testCaseName,requestBody);
    addTruthValueReq addTruthValueReq;
    addTruthValueReq.expectedOutcome = req.expectedOutcome;
    addTruthValueReq.testCaseName = req.testCaseName;
    addTruthValueReq.unparse();
    addTruthValueService(addTruthValueReq);
    resp.caseId = req.caseId;
    return resp;
}