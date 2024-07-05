#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"

void addTruthValue(HttpRequest &req) {
    cout << "addTruthValue" << endl;
    addTruthValueReq req_;
    addTruthValueResp resp;
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
        addTruthValueService(req_);
        resp.code = HTTP_OK;
        resp.msg = "addTruthValue success";
    } catch (parseExpection e) {
        resp.code = HTTP_BAD_REQUEST;
        resp.msg = e.what();
    } catch (connectExpection e) {
        resp.code = HTTP_INTERNAL_SERVER_ERROR;
        resp.msg = e.what();
    } catch (fileNotFoundException e) {
        resp.code = HTTP_NOT_FOUND;
        resp.msg = e.what();
    }
    resp.unparse();
    req.setResponse(resp.code, resp.json);
    return;
}