#pragma once
#include "../model/model.h"
#include "../store/store.h"
getTemplateResp getTemplateService(getTemplateReq req){
    getTemplateResp resp;
    resp.json = getTemplateStore(req.templateName);
    resp.parse();
    return resp;
}