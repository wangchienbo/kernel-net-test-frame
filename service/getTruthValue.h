#pragma once
#include "../model/model.h"
#include "../store/store.h"

getTruthValueResp getTruthValueService(getTruthValueReq req){
    getTruthValueResp resp;
    resp.json = getTruthValueStore(req.testCaseName);
    resp.parse();
    return resp;
}