#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include "filePathLock.h"
#include "../threadGroup.h"
#include <dirent.h>
#include <iostream>
#include <vector>
#include <cstring>

const int limitSizeWithFile = 1024 * 10; // 10KB
std::string readFileContent(const std::string& filePath) {
    FilePathLock::get_instance().lockRead(filePath);
    // cout<<"read file content:"<<endl;
    std::ifstream fileStream(filePath);
    std::ostringstream contentStream;
    contentStream << fileStream.rdbuf(); // 将文件内容读入到字符串流中
    fileStream.close();
    FilePathLock::get_instance().unlockRead(filePath);
    // cout<<"read file content end:"<<endl;
    return contentStream.str(); // 将字符串流的内容转换为字符串并返回
}
void writeFileContent(const std::string& filePath, const std::string& content, bool isAppend = false) {
    auto&g=ThreadGroup::get_instance();
    g.addThread(filePath,[](const std::string& filePath, const std::string& content, bool isAppend) {
        if(isAppend){
            // cout<<"append file content:"<<endl;
            FilePathLock::get_instance().lock(filePath);
            std::ofstream fileStream(filePath, std::ios::out | std::ios::app);
            fileStream << content;
            fileStream.close();
            FilePathLock::get_instance().unlock(filePath);
        }else{
            FilePathLock::get_instance().lock(filePath);
            // cout<<"write file content:"<<endl;
            std::ofstream fileStream(filePath);
            fileStream << content;
            fileStream.close();
            // cout<<"write file content end:"<<endl;
            FilePathLock::get_instance().unlock(filePath);
        }
    }, filePath, content,isAppend);
}

// 读取文件夹下所有文件名
std::vector<std::string> readDirectoryFilenames(const std::string& directoryPath) {
    std::vector<std::string> filenames;
    DIR* dirp = opendir(directoryPath.c_str());
    struct dirent* dp;

    if (dirp == nullptr) {
        std::cerr << "Failed to open directory: " << directoryPath << std::endl;
        return filenames; // 返回空的文件名列表
    }

    while ((dp = readdir(dirp)) != nullptr) {
        // 跳过"."和".."目录
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        // 只考虑常规文件
        if (dp->d_type == DT_REG) {
            filenames.push_back(dp->d_name);
        }
    }

    closedir(dirp);
    return filenames;
}
std::vector<std::string> readDirectoryDirNames(const std::string& directoryPath) {
    std::vector<std::string> filenames;
    DIR* dirp = opendir(directoryPath.c_str());
    struct dirent* dp;

    if (dirp == nullptr) {
        std::cerr << "Failed to open directory: " << directoryPath << std::endl;
        return filenames; // 返回空的文件名列表
    }

    while ((dp = readdir(dirp)) != nullptr) {
        // 跳过"."和".."目录
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        // 只考虑常规文件
        if (dp->d_type == DT_DIR) {
            filenames.push_back(dp->d_name);
        }
    }

    closedir(dirp);
    return filenames;
}