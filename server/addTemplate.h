#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"
// extern void AddTest_(const HttpRequest& req);
void addTemplate(HttpRequest& req){
    cout<<"addTemplate"<<endl;
    addTemplateReq req_;
    addTemplateResp resp_;
    req_.json = req.getBody();
    req_.parse();
    string data = addTemplateService(req_);
    if(data=="Invalid test type"){
        resp_.code = 400;
        resp_.data = data;
    }
    else{
        resp_.code = 200;
        resp_.data = data;
    }
    resp_.unparse();
    req.setResponse(resp_.json);
    return;
}