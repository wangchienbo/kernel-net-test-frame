#pragma once
#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"
// extern void AddTest_(const HttpRequest& req);
void runTest(HttpRequest &req) {
    runTestReq req_;
    req_.fd = req.fd;
    runTestResp resp;
    req_.json = req.getBody();
    try {
        req_.parse();
    } catch (parseExpection e) {
        resp.code = HTTP_BAD_REQUEST;
        resp.msg = e.what();
        resp.unparse();
        req.setResponse(resp.code, resp.json);
        return;
    }
    try {
        resp = runTestService(req_);
        resp.code = HTTP_OK;
        if (resp.msg.empty()) resp.msg = "runTest success";
    } catch (parseExpection e) {
        resp.code = HTTP_BAD_REQUEST;
        resp.msg = e.what();
    } catch (fileNameValidExpection e) {
        resp.code = HTTP_INTERNAL_SERVER_ERROR;
        resp.msg = e.what();
    }
    resp.unparse();
    req.setResponse(resp.code, resp.json);
    return;
}