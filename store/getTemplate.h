#pragma once
#include "../model/model.h"
#include <fstream>
#include <sys/stat.h>
#include "../common/common.h"
#include "../common/case2apiName.h"
#include "../common/http/httpResponse.h"
#include "../common/myExpection/myExpection.h"
using namespace std;
string getTemplateStore(string templateName) {
    string dir = "templates";
    cout << "getTemplateStore: " << templateName << endl;
    if (!is_file_exists(dir + "/" + templateName + ".txt")) {
        throw fileNotFoundException("TEMPLATE_NOT_EXIST，templateName: " + templateName);
    }
    return readFileContent(dir + "/" + templateName + ".txt");
}