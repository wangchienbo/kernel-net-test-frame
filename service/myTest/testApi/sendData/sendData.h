#pragma once
#include "../api.h"
#include "inputSendData.h"
#include "outputSendData.h"
// 修改函数返回类型为void，因为原代码中没有返回传输效率的逻辑
string testConnection(const std::string& ip, int port, int sizeBytes) {
    string result;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    cout<<"sock:"<<sock<<endl;
    if (sock < 0) {
        result = "Socket creation failed.";
        return result;
    }
    cout<<"sock:"<<sock<<endl;  

    // 设置连接超时
    struct timeval timeout;
    timeout.tv_sec = 10; // 10秒超时
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 尝试连接，最多重试5次，每次间隔1秒
    int attempts = 0;
    const int max_attempts = 5;
    while (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        if (++attempts >= max_attempts) {
            result = "Connection failed.";
            result += "传输速度为0B/s\n";
            std::cout << "connect failed" << std::endl;
            close(sock);
            return result;
        }
        std::cout << "Waiting for server, attempt " << attempts << std::endl;
        sleep(1); // 等待1秒再次尝试
    }
    std::cout << "Connection successful." << std::endl;
    auto start = std::chrono::steady_clock::now(); // 记录开始时间
    // 创建一个指定大小的数据包
    std::vector<char> data(sizeBytes, 'A'); // 使用'A'填充数据包

    // 发送数据
    ssize_t sentBytes = send(sock, data.data(), data.size(), 0);
    if (sentBytes < 0) {
        std::cerr << "Failed to send data." << std::endl;
    } else {
        std::cout << "Sent " << sentBytes << " bytes." << std::endl;
        // 这里可以根据发送的字节数和总时间来计算传输效率
    }
    cout<<"sentBytes:"<<sentBytes<<endl;
    auto end = std::chrono::steady_clock::now(); // 记录结束时间
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (duration == 0) duration = 1; // 防止除以0，至少为1ms

    result += "Time taken: " + std::to_string(duration) + "ms\n";
    result += "Sent " + std::to_string(sentBytes) + " bytes.\n";
    result+=" 传输速度为"+std::to_string((1.0*sentBytes/duration)/1000)+"B/s\n";
    close(sock);
    cout<<"result:"<<result<<endl;
    return result;
}
API_CLASS {
    cout << "TestAdd::Run" << endl;
    auto ip = input->targetIp;
    auto port = input->targetPort;
    auto sizeBytes = input->dataSize;
    output->targetIp = ip;
    output->targetPort = port;
    output->result = testConnection(ip, port, sizeBytes);
    cout << "TestAdd::Run end" << endl;
    return 1;
}

void API_NAME::SetUp() {
    // 准备测试环境
    cout << "TestAdd::SetUp" << endl;
}

void API_NAME::TearDown() {
    // 清理测试环境
    cout << "TestAdd::TearDown" << endl;
}