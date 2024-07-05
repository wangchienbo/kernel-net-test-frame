#pragma once
#include "../model/model.h"
#include "../store/store.h"
getTestResp getTestService(getTestReq req) {
    getTestResp resp;
    resp.data = getTestStore(req.apiName, req.testCaseName);
    return resp;
}
