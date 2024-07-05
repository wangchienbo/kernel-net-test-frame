#pragma once
#include "../model/model.h"
#include "../store/store.h"

void addTruthValueService(addTruthValueReq &req) {
    addTruthValueResp resp;
    string requestBody = req.json;
    addTruthValueStore(req.testCaseName, requestBody);
    return;
}