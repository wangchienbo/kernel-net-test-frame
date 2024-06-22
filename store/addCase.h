#pragma once
#include "../model/model.h"
#include <fstream>
#include <sys/stat.h>
#include "../common/common.h"

int addAddCaseStore(std::string apiName, std::string caseName, std::string caseBody) {
    std::cout << "Adding Case: " << endl;
    string path="store/"+apiName;
    if(is_directory_exists(path) == false) {
        create_directory(path);
    }
    cout << "path: " << path << endl;
    if (case2apiName::get_instance().getdata(caseName)!=""){
        return ExistErrorCode;
    }    
    writeFileContent(path+"/"+caseName+".txt",caseBody,true);
    auto&g=ThreadGroup::get_instance();
    g.wait(path+"/"+caseName+".txt");
    case2apiName::get_instance().setdata(caseName,apiName);
    return SUCCESSCode;
}