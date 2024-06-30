#pragma once
#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"

void getTruthValue(HttpRequest& req) {
    cout << "getTruthValue" << endl;
    getTruthValueReq req_;
    req_.json = req.getBody();
    getTruthValueResp resp;
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
        resp = getTruthValueService(req_);
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
    cout << "getTruthValueResp: " << resp.data << endl;
    resp.unparse();
    req.setResponse(resp.code, resp.json);
    return ;
}