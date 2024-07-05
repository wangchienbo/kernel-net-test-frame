#pragma once
#include "../common/common.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;
using namespace std;
std::string argsToJson(const std::string &args) {
    std::istringstream iss(args);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    std::string json = "{";
    for (size_t i = 0; i < tokens.size(); i += 2) {
        std::string key = tokens[i];
        if (key[0] == '-' && i + 1 < tokens.size()) {
            std::string value = tokens[i + 1];
            key = key.substr(1); // 移除键前的 '-'
            // 为键和值添加引号，并用冒号和逗号分隔
            json += "\"" + key + "\":\"" + value + "\"";
            if (i + 2 < tokens.size()) json += ",";
        }
    }
    json += "}";
    if (json == "{}") {
        return args;
    }
    return json;
}
vector<std::string> extractDataBetweenFlags(const std::string &input, const std::string &flag) {
    size_t startPos = input.find(flag);
    vector<std::string> res;
    if (startPos != std::string::npos) {
        startPos += flag.length();                  // 移动到标志之后的第一个字符
        size_t endPos = input.find(" -", startPos); // 从标志之后开始查找下一个"-"

        if (endPos == std::string::npos) { // 如果没有找到，取到字符串末尾
            res = splitString(input.substr(startPos), " ");
        } else {
            res = splitString(input.substr(startPos, endPos - startPos), " ");
        }
    }
    for (auto &s : res) {
        if (s.size() > 0 && (s[0] == '\'' || s[0] == '"')) {
            s = s.substr(1, s.size() - 1);
        }
        if (s.size() > 0 && s[s.size() - 1] == '\'' || s[s.size() - 1] == '"') {
            s = s.substr(0, s.size() - 1);
        }
    }

    return res; // 如果没有找到标志，返回空字符串
}
std::map<std::string, std::string> parse(const std::string &str) {
    map<string, string> res;
    std::istringstream sstr(str);
    // Parse the request line
    string version, code, status;
    std::getline(sstr, version, ' ');
    std::getline(sstr, code, ' ');
    std::getline(sstr, status, '\n');

    // Normalize the version by removing "\r"
    status.erase(std::remove(status.begin(), status.end(), '\r'), status.end());
    std::string line;
    std::map<std::string, std::string> headers;
    while (std::getline(sstr, line, '\n') && line != "\r") {
        auto colon = line.find(':');
        if (colon != std::string::npos) {
            auto name = line.substr(0, colon);
            auto pos = colon + 1;
            while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) {
                ++pos;
            }
            auto value = line.substr(pos);
            value.erase(std::remove(value.begin(), value.end(), '\r'), value.end()); // Remove any '\r'
            headers[name] = value;
        }
    }

    for (const auto &header : headers) {
        // std::cout << header.first << ": " << header.second << std::endl;
    }
    string body;
    body = std::string(std::istreambuf_iterator<char>(sstr), {});
    res["version"] = version;
    res["code"] = code;
    res["status"] = status;
    res["body"] = body;
    return res;
}
bool isValidIP(const std::string &ip) {
    std::stringstream ss(ip);
    std::string segment;
    int count = 0;
    while (std::getline(ss, segment, '.')) {
        count++;
        int num = std::stoi(segment);
        if (num < 0 || num > 255) {
            return false;
        }
    }
    return count == 4;
}

bool isValidPort(const std::string &port) {
    int port_num = std::stoi(port);
    return port_num >= 0 && port_num <= 65535;
}

int main(int argc, char *argv[]) {
    // setpgid(0, 0);
    std::vector<std::pair<std::string, int>> ips;
    vector<string> ips_str;
    std::cout << "argc:" << argc << endl;
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            ips_str.push_back(argv[i]);
        }
    } else {
        ips_str.push_back("127.0.0.1:8080");
        argc = ips_str.size();
    }
    std::cout << "ips_str size: " << ips_str.size() << endl;
    for (int i = 0; i < ips_str.size(); i++) {
        std::string arg = ips_str[i];
        size_t pos = arg.find(':');
        if (pos != std::string::npos) {
            std::string ip = arg.substr(0, pos);
            std::string port = arg.substr(pos + 1);
            if (isValidIP(ip) && isValidPort(port)) {
                ips.push_back(std::make_pair(ip, std::stoi(port)));
            } else {
                std::cout << "ip或端口错误,ip：" << ip << "端口：" << port << endl;
                return 0;
            }
        }
    }
    vector<httpClient *> clients;
    int a;
    int n = ips.size();
    for (int i = 0; i < n; i++) {
        std::pair<std::string, int> ip = ips[i];
        httpClient *c = new httpClient(ip.first, ip.second);
        try {
            c->init();
            c->run();
        } catch (connectExpection e) {
            std::cout << "连接失败:" << e.what() << "\n";
        }
        clients.push_back(c);
    }
    std::cout << "初始化完毕";
    std::cout << "请输入要执行的命令和数据\n";
    while (1) {
        std::cout << "\n>>";
        string runName, data, s;
        std::getline(std::cin, s);
        std::istringstream sstr(s);
        sstr >> runName;
        vector<string> inputDatas;
        data = std::string(std::istreambuf_iterator<char>(sstr), {});
        auto inputIps = extractDataBetweenFlags(data, "-ips");
        auto fileNames = extractDataBetweenFlags(data, "-f");
        if (inputIps.size() != 0 && inputIps.size() != fileNames.size()) {
            std::cout << "ip和文件数量不匹配，请使用-f输入每个节点添加的模板\n";
            continue;
        }
        if (inputIps.size() == 0 && fileNames.size() > 1) {
            std::cout << "输入-f参数过多，请指定唯一请求\n";
            continue;
        }

        if (inputIps.size() == 0 && fileNames.size() == 1) {
            std::cout << "fileName: " << fileNames[0] << endl;
            try {
                inputDatas.push_back(readFileContent(fileNames[0]));
                data = inputDatas[0];
                std::cout << "inputData:" << data << endl;
            } catch (std::exception e) {
                std::cout << "读取文件失败:" << e.what() << "\n";
                continue;
            }
        }
        bool readSuccess = true;
        if (inputIps.size() != 0) {
            for (int i = 0; i < inputIps.size(); i++) {
                try {
                    std::cout << "正在读取文件：" << fileNames[i] << endl;
                    inputDatas.push_back(readFileContent(fileNames[i]));
                } catch (fileNameValidExpection e) {
                    std::cout << "读取文件失败:" << e.what() << "\n";
                    readSuccess = false;
                    break;
                } catch (fileNotFoundException e) {
                    std::cout << "读取文件失败:" << e.what() << "\n";
                    readSuccess = false;
                    break;
                } catch (std::exception e) {
                    std::cout << "读取文件失败:" << e.what() << "\n";
                    readSuccess = false;
                    break;
                }
            }
        }
        if (!readSuccess) {
            continue;
        }
        int num = 0;
        std::cout << "开始执行\n"
                  << inputIps.size() << endl;
        vector<pair<string, string>> results;
        vector<thread> threads;
        mutex m;
        if (inputIps.size() != 0) {
            for (int i = 0; i < inputIps.size(); i++) {
                threads.emplace_back([&results, clients, ips, inputIps, inputDatas, i, runName, &m]() {
                    string result = "";
                    string ip_port = inputIps[i];
                    string inputData = inputDatas[i];
                    try {
                        int idx = 0;
                        for (int j = 0; j < ips.size(); j++) {
                            if (ips[j].first + ":" + to_string(ips[j].second) == ip_port) {
                                idx = j;
                                break;
                            }
                        }
                        if (idx >= ips.size()) {
                            throw connectExpection("ip不存在:" + ip_port);
                            return;
                        }
                        string ip = ips[idx].first + ":" + to_string(ips[idx].second);
                        auto client = clients[idx];
                        string res = client->runRequest(runName, inputData);
                        Response resp = Response();
                        auto res_map = parse(res);
                        if (res_map["code"] == "200") {
                            result = "ip为：" + ip + "执行成功\n";
                            result += "结果为：" + res_map["body"];
                        } else {
                            result = "ip为：" + ip + "执行失败\n";
                            result += "结果为：" + res_map["body"];
                        }
                    } catch (connectExpection e) {
                        result += "ip为：" + ip_port + "连接失败:" + e.what() + "\n";
                    } catch (parseExpection e) {
                        result += "ip为：" + ip_port + "解析失败:" + e.what() + "\n";
                    } catch (std::exception e) {
                        result += "ip为：" + ip_port + "执行失败:" + e.what() + "\n";
                    }
                    m.lock();
                    results.push_back(make_pair(ip_port, result));
                    m.unlock();
                });
            }
            for (auto &t : threads) {
                t.join();
            }
            for (auto &r : results) {
                std::cout << "主机" << r.first << "返回结果是：" << endl;
                std::cout << r.second << endl;
            }
            continue;
        }
        data = argsToJson(data);

        for (auto &client : clients) {
            threads.emplace_back([&client, runName, data, ips, num, &results, &m]() {
                string result = "";
                string ip = ips[num].first + ":" + to_string(ips[num].second);
                try {
                    string res = client->runRequest(runName, data);
                    Response resp = Response();
                    auto res_map = parse(res);
                    if (res_map["code"] == "200") {
                        result = "ip为：" + ip + "执行成功\n";
                        result += "结果为：" + res_map["body"];
                    } else {
                        result = "ip为：" + ip + "执行失败\n";
                        result += "结果为：" + res_map["body"];
                    }
                } catch (connectExpection e) {
                    result += "ip为：" + ip + "连接失败:" + e.what() + "\n";
                } catch (parseExpection e) {
                    result += "ip为：" + ip + "解析失败:" + e.what() + "\n";
                } catch (std::exception e) {
                    result += "ip为：" + ip + "执行失败:" + e.what() + "\n";
                }
                m.lock();
                results.push_back(make_pair(ip, result));
                m.unlock();
            });
            num++;
        }
        for (auto &t : threads) {
            t.join();
        }
        for (auto &r : results) {
            std::cout << "主机" << r.first << "返回结果是：" << endl;
            std::cout << r.second << endl;
        }
    }

    return 0;
}