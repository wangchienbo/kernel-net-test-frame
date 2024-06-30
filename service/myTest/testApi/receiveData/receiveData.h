#pragma once
#include "../api.h"
#include "inputReceiveData.h"
#include "outputReceiveData.h"
std::string testConnection(int port, int sizeBytes, Input_ReceiveData* input) {
    string result = ""; 
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    std::string receivedData = "";


    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "socket failed" << std::endl;

        result = "Socket creation error";
        result+="传输速度为0B/s\n";
        return "Socket creation error";
    }
    input->listenFd = server_fd;
    // 绑定套接字到端口
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "setsockopt" << std::endl;
        result = "Setsockopt error";
        result+="传输速度为0B/s\n";
        return "Setsockopt error";
    }
    string ip="0.0.0.0";
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "bind failed" << std::endl;
        result = "Bind error";
        result+="传输速度为0B/s\n";
        return "Bind error";
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "listen" << std::endl;
        result = "Listen error";
        result+="传输速度为0B/s\n";
        return "Listen error";
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        std::cerr << "accept" << std::endl;
        result = "Accept error";
        result+="传输速度为0B/s\n";
        return "Accept error";
    }
    input->connFd = new_socket;
    auto start = std::chrono::steady_clock::now(); // 记录开始时间
    // 循环接收数据直到达到期望的大小
    int totalReceived = 0;
    while (totalReceived < sizeBytes) {
        int valread = recv(new_socket, buffer, sizeof(buffer), 0);
        if (valread <= 0) {
            // 连接关闭或出错
            break;
        }
        cout << "Received " << valread << " bytes" << endl;
        receivedData.append(buffer, valread);
        totalReceived += valread;
    }
    cout << "Total received: " << totalReceived << " bytes" << endl;
    auto end = std::chrono::steady_clock::now(); // 记录结束时间
    // 计算耗时
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // 避免除以零
    if (duration == 0) duration = 1; // 防止除以0，至少为1ms
    result += "Time taken: " + std::to_string(duration) + "ms\n";
    result += "Received " + std::to_string(totalReceived) + " bytes.\n";
    result+=" 传输速度为"+std::to_string((1.0*totalReceived/duration)/1000)+"B/s\n";
    result+="丢包率为"+std::to_string(100*(sizeBytes-totalReceived)/sizeBytes)+"%\n";
    cout<<"result:"<<result<<endl;
    return result;
}
API_CLASS {
    cout << "TestAdd::Run" << endl;
    auto port = input->targetPort;
    auto sizeBytes = input->dataSize;
    output->result = testConnection(port, sizeBytes, input);
    return 1;
}


void API_NAME::SetUp() {
    // 准备测试环境

    cout << "TestAdd::SetUp" << endl;
}

void API_NAME::TearDown() {
    // 清理测试环境
    close(input->listenFd);
    close(input->connFd);
    cout << "TestAdd::TearDown" << endl;
}