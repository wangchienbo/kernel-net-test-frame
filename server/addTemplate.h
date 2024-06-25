#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"
// extern void AddTest_(const HttpRequest& req);
void addTemplate(HttpRequest& req){
    cout<<"addTemplate"<<endl;
    addTemplateReq req_;
    addTemplateResp resp;
    req_.json = req.getBody();
    try{
        req_.parse();
    }
    catch(parseExpection e){
        resp.code = HTTP_BAD_REQUEST;
        resp.data = e.what();
        resp.unparse();
        req.setResponse(resp.code,resp.json);
        return ;
    }
    try {
        resp = addTemplateService(req_);
        resp.code = HTTP_OK;
        resp.msg = "addTemplate success";
    }
    catch(parseExpection e){
        resp.code = HTTP_BAD_REQUEST;
        resp.msg = e.what();
    }
    catch(templateExistExpection e){
        resp.code = HTTP_INTERNAL_SERVER_ERROR;
        resp.msg = e.what();
    }
    resp.unparse();
    req.setResponse(resp.code,resp.json);
    return ;
}