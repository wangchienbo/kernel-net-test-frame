#pragma once
#include <string>
#include <vector>
#include "input.h"

class TestCase {
public:
    virtual void SetUp() = 0; // 在每个测试用例前执行
    virtual void TearDown() = 0; // 在每个测试用例后执行
    virtual void startTest() = 0; // 执行测试用例的主体
    virtual void setJson(string json) = 0;
    virtual Input* getInput() = 0;
    virtual void run(string json) {
        auto input_ = this->getInput();
        input_->json = json;
        input_->parse();
        SetUp();
        startTest();
        TearDown();
    }
    string json;
    string apiName;
    Input *input;
    TestCase() {}
    virtual ~TestCase() {}
};