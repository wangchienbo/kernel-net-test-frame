#pragma once
#include <vector>
#include "testCase.h"
#include <memory>
#include <map>
// 测试用例注册表
class TestCaseRegistry {
private:
    std::map<std::string, std::shared_ptr<TestCase>> test_cases_;

public:
    void RegisterTestCase(TestCase *test_case) {
        test_cases_[test_case->apiName] = std::shared_ptr<TestCase>(test_case);
    }
    std::map<std::string, std::shared_ptr<TestCase>> GetTestCases() const {
        return test_cases_;
    }
    static TestCaseRegistry &Instance() {
        static TestCaseRegistry instance;
        return instance;
    }
};