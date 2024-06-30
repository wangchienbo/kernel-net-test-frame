#pragma once
#include "../model/model.h"
#include <fstream>
#include <sys/stat.h>
#include "../common/common.h"
#include "../common/case2apiName.h"
#include "../common/http/httpResponse.h"
#include "../common/myExpection/myExpection.h"
using namespace std;
string getTruthValueStore(string testCaseName) {
    string res = "";
    string dir = "truthValues";
    cout << "getTestStore: "<< testCaseName << endl;
    // casename为空，不执行
    if (testCaseName == "") {
        throw parseExpection(INVALID_PARAM);
    }
    // 查询真值，只需要判断是否存在，然后返回
    if (is_file_exists(dir + "/" + testCaseName + ".txt")) {
        string content = readFileContent(dir + "/" + testCaseName + ".txt");
        res = content;
    }
    return res;
}