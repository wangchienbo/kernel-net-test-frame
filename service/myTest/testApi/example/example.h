#pragma once
#include "../api.h"
#include "inputExample.h"
#include "outputExample.h"

// 定义和注册TestAdd的测试用例
API_CLASS {
    auto ip = *input;
    output->sd1 = ip.sd1;
    output->sd2 = ip.sd2;
    output->temp.num = ip.temp.num;
    output->temp.abs = ip.temp.abs;
    output->sd3 = ip.sd3;
    for(auto temp: ip.temps){
        Temp2 temp2;
        temp2.num = temp.num;
        temp2.abs = temp.abs;
        output->temps.push_back(temp2);
    }
    cout << "TestAdd::Run" << endl;
    return 1;
}


void API_NAME::SetUp() {
    // 准备测试环境
    cout << "TestAdd::SetUp" << endl;
}

void API_NAME::TearDown() {
    // 清理测试环境
    cout << "TestAdd::TearDown" << endl;
}