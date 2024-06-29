#pragma once
#include "../model/model.h"
#include <fstream>
#include <sys/stat.h>
#include "../common/common.h"

void addTruthValueStore(std::string caseName, std::string requestBody) {
    std::cout << "Adding Truth Value: " << endl;
    string path="truthValues/";
    if(is_directory_exists(path) == false) {
        create_directory(path);
    }
    cout << "path: " << path << endl;
    writeFileContent(path + caseName + ".txt", requestBody, false);
    auto&g=ThreadGroup::get_instance();
    g.wait(path + ".txt");
    return ;
}