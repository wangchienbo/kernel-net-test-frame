#include "../api.h"
#include "inputExample.h"

// 定义和注册TestAdd的测试用例
API_CLASS {
    auto ip = *input;
    cout << ip.sd1 << endl;
    cout << ip.sd2 << endl;
    cout << ip.temp.num << endl;
    cout << ip.temp.abs << endl;
    cout << "TestAdd::Run" << endl;
}


void API_NAME::SetUp() {
    // 准备测试环境
    cout << "TestAdd::SetUp" << endl;
}

void API_NAME::TearDown() {
    // 清理测试环境
    cout << "TestAdd::TearDown" << endl;
}