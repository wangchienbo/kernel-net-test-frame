#pragma once
#include "../model/model.h"
#include <fstream>
#include <sys/stat.h>
#include "../common/common.h"
#include "../common/case2apiName.h"
#include "../common/http/httpResponse.h"
#include "../common/myExpection/myExpection.h"
void saveReportStore(std::string reportName, std::string report) {
    std::cout << "Saving Report: " << endl;
    string path="reports";
    if(is_directory_exists(path) == false) {
        create_directory(path);
    }
    cout << "path: " << path << endl;
    writeFileContent(path+"/"+reportName+".txt",report,true);
}