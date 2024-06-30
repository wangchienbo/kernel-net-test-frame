#pragma once
#include "../model/model.h"
#include "../store/store.h"

getTruthValueResp getTruthValueService(getTruthValueReq req){
    getTruthValueResp resp;
    resp.data = getTruthValueStore(req.testCaseName);
    return resp;
}