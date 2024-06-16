#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"
// extern void AddTest_(const HttpRequest& req);
void addTemplate(HttpRequest& req){
    cout<<"addTemplate"<<endl;
    addTemplateReq req_;
    addTemplateResp resp_;
    req_.json = req.getBody();
    try{
        req_.parse();
    }
    catch(parseExpection e){
        resp_.code = 400;
        resp_.data = e.what();
        resp_.unparse();
        req.setResponse(resp_.code,resp_.json);
        return ;
    }
    string data = addTemplateService(req_);
    if(data=="Invalid test type"){
        resp_.code = 500;
        resp_.data = data;
    }
    else{
        resp_.code = 200;
        resp_.data = data;
    }
    resp_.unparse();
    req.setResponse(resp_.code,resp_.json);
    return ;
}