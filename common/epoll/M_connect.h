#pragma once
#include "head.h"
#include "mystruct.h"
#define buffsize 1024
void error_handling(char *message) {
    perror(message);
    exit(1);
}
class myconnect {
public:
    struct sockaddr_in service_addr;
    struct in_addr go_ip;
    u_int16_t go_port;
    char buffer[buffsize];
    int sockfd;
    myconnect(string go_ip_s, int go_port_s) {
        cout << "初始化" << go_ip_s << go_port_s << endl;
        go_ip.s_addr = inet_addr((char *)go_ip_s.c_str());
        go_port = htons(go_port_s);
        cout << "初始化完毕" << go_port << endl;
    }
    int init() {
        memset(&service_addr, 0, sizeof(service_addr));
        service_addr.sin_family = AF_INET;
        service_addr.sin_port = go_port;
        service_addr.sin_addr.s_addr = go_ip.s_addr;
        return sockfd = socket(AF_INET, SOCK_STREAM, 0);
    }
    int run() {
        service_addr.sin_addr.s_addr = go_ip.s_addr;
        if (connect(sockfd, (struct sockaddr *)&service_addr, sizeof(service_addr)) < 0) {
            throw connectExpection("连接失败");
        }
        return 1;
    }
    int myread(string &res) {
        int num = 0;
        num = read(sockfd, buffer, buffsize - 1);
        while (num > 0) {
            for (int i = 0; i < num; i++) {
                if (buffer[i] == '#') {
                    return 1;
                }
                res += buffer[i];
            }
            num = read(sockfd, buffer, buffsize - 1);
        }
        throw connectExpection("读取失败");
    }
    int mywrite(string Request) {
        int error = 0;
        socklen_t len = sizeof(error);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) == 0) {
            if (errno != 0) {
                cout << "未连接，尝试重新连接..." << endl;
                init();
                if (run() < 0) {
                    throw connectExpection("重新连接失败");
                }
            }
        }
        int length = Request.size();
        char buf[length + 2];
        for (int i = 0; i < length; i++) {
            buf[i] = Request[i];
        }
        buf[length] = '#';
        buf[length + 1] = '\0';
        if (send(sockfd, buf, length + 1, 0) < 0) {
            throw connectExpection("发送失败");
        }
        return 1;
    }

    ~myconnect() {
        close(sockfd);
    }
};