#pragma once
#include "../common/http/http.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
using namespace std;
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
    setpgid(0, 0);
    std::vector<std::pair<std::string, int>> ips;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
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
    std::cout << "请输入要执行的命令和数据\n>>";
    while (1) {
        int num = 0;
        string runName, data, s;
        std::getline(std::cin, s);
        std::istringstream sstr(s);
        sstr >> runName;
        data = std::string(std::istreambuf_iterator<char>(sstr), {});
        while (data.size() > 0 && data[0] == ' ') {
            data = data.substr(1);
        }
        for (auto &client : clients) {
            string ip = ips[num].first + ":" + to_string(ips[num].second);
            try {
                string res = client->runRequest(runName, data);
                Response resp = Response();
                auto res_map = parse(res);
                if (res_map["status"] == "OK") {
                    std::cout << "ip为：" << ip << "执行成功\n";
                    std::cout << "结果为：" << res_map["body"] << endl;
                } else {
                    std::cout << "ip为：" << ip << "执行失败\n";
                    std::cout << "结果为：" << res_map["body"] << endl;
                }
            } catch (connectExpection e) {
                std::cout << "ip为：" << ip << "连接失败:" << e.what() << "\n";
            }
            num++;
        }
        std::cout << "\n>>";
    }

    return 0;
}