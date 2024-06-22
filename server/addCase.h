#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"
// extern void AddTest_(const HttpRequest& req);
void addCase(HttpRequest& req){
    cout<<"addCase"<<endl;
    addCaseReq req_;
    addCaseResp resp_;
    req_.json = req.getBody();
    try{
        req_.parse();
    }
    catch(parseExpection e){
        resp_.code = HTTP_BAD_REQUEST;
        resp_.data = e.what();
        resp_.unparse();
        req.setResponse(resp_.code,resp_.json);
        return ;
    }
    int res = addCaseService(req_);
    if(res==ExistErrorCode){
        resp_.code = HTTP_INTERNAL_SERVER_ERROR;
        resp_.data = "Case already exists";
    }
    else{
        resp_.code = HTTP_OK;
        resp_.data = "Case added successfully";
    }
    resp_.unparse();
    req.setResponse(resp_.code,resp_.json);
    return ;
}