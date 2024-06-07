#pragma once
#include "testCaseRegistry.h"

// 用于创建临时对象并注册测试用例的辅助类
class TestCaseRegistrar {
public:
    TestCaseRegistrar(TestCase* test_case) {
        TestCaseRegistry::Instance().RegisterTestCase(test_case);
    }
};
// 定义宏，用于声明和注册测试用例
#define TEST_CASE(name) \
    class name : public TestCase { \
    public: \
        Input_##name *input;\
        name() {\
            apiName = #name; \
            input = new Input_##name(); \
        } \
        void SetUp() override; \
        void TearDown() override; \
        void startTest() override; \
        void setJson(string json) { \
            this->input->json = json; \
        } \
        Input* getInput() override { \
            return dynamic_cast<Input*>(input); \
        } \
    }; \
    static TestCaseRegistrar registrar_##name(new name()); \
    void name::startTest()