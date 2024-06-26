#pragma once
#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"
void getTemplate(HttpRequest& req) {
    cout << "getTemplate" << endl;
    getTemplateReq req_;
    req_.json = req.getBody();
    getTemplateResp resp;
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
        resp = getTemplateService(req_);
        resp.code = 200;
    } catch (parseExpection e) {
        resp.code = 400;
        resp.msg = e.what();
        resp.msg="template format parse error"+resp.msg;
    } catch (connectExpection e) {
        resp.code = 500;
        resp.msg = e.what();
    } catch (fileNotFoundException e) {
        resp.code = 404;
        resp.msg = e.what();
    }
    cout << "getTemplateResp: " << resp.data.size() << endl;
    resp.unparse();
    req.setResponse(resp.code, resp.json);
    return ;
}
