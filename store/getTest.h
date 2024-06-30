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
    string dir = "cases";
    cout << "getTestStore: " << apiName << " " << testCaseName << endl;
    // casename为空，不执行
    // apiname 为空，返回空
    if (apiName == "" && testCaseName == "") {
        throw parseExpection(INVALID_PARAM);
    }
    if (testCaseName != "") {
        cout << "testCaseName: " << testCaseName << endl;
        string apiName_ = case2apiName::get_instance().getdata(testCaseName);
        cout << "apiName_: " << apiName_ << endl;
        if (apiName_ == "") {
            cout << "apiName_ is empty" << endl;
            throw fileNotFoundException(CASE_NOT_EXIST);
        }
        cout << "正在读取文件" << dir + "/" + apiName_ + "/" + testCaseName + ".txt" << endl;
        if (is_file_exists(dir + "/" + apiName_ + "/" + testCaseName + ".txt")) {
            cout << "path: " << dir + "/" + apiName_ + "/" + testCaseName + ".txt" << endl;
            string content = readFileContent(dir + "/" + apiName_ + "/" + testCaseName + ".txt");
            cout << "content: " << content << endl;
            res.push_back(content);
        }
        cout<<"res: "<<res.size()<<endl;
        
    } else {
        cout << "apiName: " << apiName << endl;
        if(is_directory_exists(dir + "/" + apiName) == false) {
            throw fileNotFoundException(CASE_NOT_EXIST);
        }
        cout << "dir: " << dir + "/" + apiName << endl;
        vector<string> filenames = readDirectoryFilenames(dir + "/" + apiName);
        cout << "filenames: " << filenames.size() << endl;
        vector<string> res;
        for (const auto& filename : filenames) {
            if (filename.find(".txt") != string::npos) {
                cout << "filename: " << filename << endl;
                 res.push_back(readFileContent(dir + "/" + apiName + "/" + filename));
            }
        }
        return res;
    }
    return res;
}