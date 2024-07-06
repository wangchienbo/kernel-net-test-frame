#pragma once
#include "../model/model.h"
#include <fstream>
#include <sys/stat.h>
#include "../common/common.h"

void addTemplateStore(std::string templateName, std::string templatebody) {
    std::cout << "Adding template: " << templateName << std::endl;
    std::cout << "Template body: " << templatebody << std::endl;
    std::string dir = "templates";
    if (is_directory_exists(dir) == false) {
        create_directory(dir);
    }
    if (is_file_exists(dir + "/" + templateName + ".txt")) {
        throw templateExistExpection("Template file already exist: " + templateName);
    }
    writeFileContent(dir + "/" + templateName + ".txt", templatebody);
    ThreadGroup::get_instance().wait(dir + "/" + templateName + ".txt");
    std::cout << "Template added successfully" << std::endl;
}