#pragma once
#include "../model/model.h"
#include <fstream>
#include <sys/stat.h>

void addTemplateStore(std::string templateName, std::string templatebody) {
    std::cout << "Adding template: " << templateName << std::endl;
    std::cout << "Template body: " << templatebody << std::endl;
    std::string dir = "templates";
    if(is_directory_exists(dir) == false) {
        create_directory(dir);
    }
    std::ofstream file(dir+ "/" + (templateName + ".txt"), std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "File could not be opened for writing\n";
        return;
    }

    file.write(templatebody.c_str(), templatebody.size());
    file.close();

    std::cout << "Template added successfully" << std::endl;
}