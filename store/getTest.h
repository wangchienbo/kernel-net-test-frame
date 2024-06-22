#pragma once
#include "../model/model.h"
#include <fstream>
#include <sys/stat.h>
#include "../common/common.h"
#include "../common/case2apiName.h"
#include "../common/http/httpResponse.h"
#include "../common/myExpection/myExpection.h"
using namespace std;
vector<string> getTestStore(string apiName, string testCaseName) {
    vector<string>res;
    string dir = "case";
    // casename为空，不执行
    // apiname 为空，返回空
    if (apiName == "" && testCaseName == "") {
        throw parseExpection(INVALID_PARAM);
    }

    if (testCaseName != "") {
        string apiName_ = case2apiName::get_instance().getdata(testCaseName);
        if (apiName_ == "") {
            throw fileNotFoundException(CASE_NOT_EXIST);
        }
        res.push_back(readFileContent(dir + "/" + apiName_ + "/" + testCaseName + ".txt"));
    } else {

        vector<string> filenames = readDirectoryFilenames(dir + "/" + apiName);
        vector<string> res;
        for (const auto& filename : filenames) {
            if (filename.find(".txt") != string::npos) {
                 res.push_back((dir + "/" + apiName + "/" + filename));
            }
        }
        return res;
    }

}

bool is_directory_exists(const string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return S_ISDIR(buffer.st_mode);
    } else {
        return false;
    }
}