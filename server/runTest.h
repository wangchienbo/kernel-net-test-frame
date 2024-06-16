#include "../common/common.h"
#include "../model/model.h"
#include "../service/service.h"
// extern void AddTest_(const HttpRequest& req);
void RunTest(HttpRequest& req){
    cout<<"AddTest"<<endl;
    runTestReq req_;
    runTestResp resp_;
    req_.json = req.getBody();
    cout<<req_.json<<endl;
    req_.parse();
    cout<<req_.apiName<<endl;
    cout<<req_.input<<endl;
    auto myApiRun=ApiRun();
    apiRunResp res= myApiRun.run(req_.apiName,req_.input);

    resp_.code = res.code;
    resp_.data = res.data;
    resp_.unparse();
    req.setResponse(resp_.code,resp_.json);
    return;
}