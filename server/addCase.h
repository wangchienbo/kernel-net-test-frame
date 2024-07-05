#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"
// extern void AddTest_(const HttpRequest& req);
void addCase(HttpRequest &req) {
    cout << "addCase" << endl;
    addCaseReq req_;
    addCaseResp resp;
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
        resp = addCaseService(req_);
        resp.code = HTTP_OK;
        resp.msg = "addCase success";
    } catch (parseExpection e) {
        resp.code = HTTP_BAD_REQUEST;
        resp.msg = e.what();
    } catch (connectExpection e) {
        resp.code = HTTP_INTERNAL_SERVER_ERROR;
        resp.msg = e.what();
    } catch (fileNotFoundException e) {
        resp.code = HTTP_NOT_FOUND;
        resp.msg = e.what();
    } catch (caseExistExpection e) {
        resp.code = HTTP_INTERNAL_SERVER_ERROR;
        resp.msg = e.what();
    }
    resp.unparse();
    req.setResponse(resp.code, resp.json);
    return;
}