#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"

void getTest(HttpRequest& req) {
    cout << "getTest" << endl;
    getTestReq req_;
    req_.json = req.getBody();
    getTestResp resp;
    try {
        req_.parse();

    } catch(parseExpection e) {
        resp.code = 400;
        resp.msg = e.what();
        resp.unparse();
        req.setResponse(resp.code, resp.json);
        return ;
    }
    try {
        resp = getTestService(req_);
        resp.code = 200;
    } catch (parseExpection e) {
        resp.code = 400;
        resp.msg = e.what();
    } catch (connectExpection e) {
        resp.code = 500;
        resp.msg = e.what();
    } catch (fileNotFoundException e) {
        resp.code = 404;
        resp.msg = e.what();
    }
    resp.unparse();
    req.setResponse(resp.code, resp.json);
    return ;
}