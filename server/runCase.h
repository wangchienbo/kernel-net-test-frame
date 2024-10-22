#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"
#include "model/apiRunResp.h"
void runCase(HttpRequest &req) {
    runCaseReq req_;
    req_.fd = req.fd;
    runCaseResp resp;
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
        resp = runCaseService(req_);
        resp.code = HTTP_OK;
        if(resp.testResult.responseCode != SUCCESSCode){
            resp.code = HTTP_INTERNAL_SERVER_ERROR;
            resp.msg = "runCase failed";
        }
        if (resp.msg.empty()) resp.msg = "runCase success";
    } catch (parseExpection e) {
        resp.code = HTTP_BAD_REQUEST;
        resp.msg = e.what();
    } catch (fileNameValidExpection e) {
        resp.code = HTTP_INTERNAL_SERVER_ERROR;
        resp.msg = e.what();
    } catch (const std::exception &e) {
        resp.code = HTTP_INTERNAL_SERVER_ERROR;
        resp.msg = e.what();
    }
    resp.unparse();
    req.setResponse(resp.code, resp.json);
    return;
}