#pragma once
#include "../model/model.h"
#include <fstream>
#include <sys/stat.h>

bool is_directory_exists(const std::string &path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return S_ISDIR(buffer.st_mode);
    } else {
        return false;
    }
}

void create_directory(const std::string &path) {
    if (!is_directory_exists(path)) {
        mkdir(path.c_str(), 0777);
    }
}
void addTemplateStore(std::string templateName, std::string templatebody) {
    std::cout << "Adding template: " << templateName << std::endl;
    std::cout << "Template body: " << templatebody << std::endl;
    std::string dir = "templates";
    if (is_directory_exists(dir) == false) {
        create_directory(dir);
    }
    std::ofstream file(dir + "/" + (templateName + ".txt"), std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "File could not be opened for writing\n";
        return;
    }

    file.write(templatebody.c_str(), templatebody.size());
    file.close();

    std::cout << "Template added successfully" << std::endl;
}