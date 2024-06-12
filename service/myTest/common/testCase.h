#pragma once
#include <string>
#include <vector>
#include "input.h"
#include "output.h"
#include "../../../model/model.h"

class TestCase {
public:
    virtual void SetUp() = 0; // 在每个测试用例前执行
    virtual void TearDown() = 0; // 在每个测试用例后执行
    virtual int startTest() = 0; // 执行测试用例的主体
    virtual void setJson(string json) = 0;
    virtual void clear() = 0;
    virtual Input* getInput() = 0;
    virtual Output* getOutput() = 0;
    virtual apiRunResp* run(string json) {
        clear();
        auto resp = new apiRunResp();
        auto input_ = this->getInput();
        input_->json = json;
        input_->parse();
        SetUp();
        bool isSucess=startTest();
        TearDown();
        auto output_ = this->getOutput();
        if(isSucess){
            resp->code = SUCCESSCode;
            output_->unparse();
            int resLen=output_->json.length();
        } else {
            resp->code = testErrorCode;
        }
        resp->data = output_->json;
        return resp;
    }
    string json;
    string apiName;
    Input *input;
    Output *output;
    TestCase() {}
    virtual ~TestCase() {}
};