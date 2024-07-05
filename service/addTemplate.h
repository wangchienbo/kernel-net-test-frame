#pragma once
#include "../model/model.h"
#include "../store/store.h"
addTemplateResp addTemplateService(addTemplateReq req) {
    addTemplateResp resp;
    req.templateId = req.templateName + "_" + getCurrentTime();
    req.json.clear();
    req.unparse();
    string requestBody = req.json;
    addTemplateStore(req.templateName, req.json);
    resp.templateId = req.templateId;
    return resp;
}