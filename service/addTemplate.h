#pragma once
#include "../model/model.h"
#include "../store/store.h"
string addTemplateService(addTemplateReq req){
    addTemplateResp resp;
    string templateBody = req.json;
    if(req.testType != "bench" && req.testType != "single"){
        return "Invalid test type";
    }
    addTemplateStore(req.templateName,templateBody);
    return "Template added successfully";
}