#pragma once
#include "head.h"
#include "messageQueue.h"
#include "messageThreadPool.h"
#include "responseThreadPool.h"
#include "../fdStatus.h"

class epollServer {
public:
    struct sockaddr_in service_addr;
    int listenfd;
    int epollfd; // 使用epoll
    static const int MAX_EVENTS = 10;
    
    ThreadPool* pool;
    int port;
    epollServer(ResponseThreadPool* responsePool_, int port) : port(port) {
        pool = new ThreadPool(responsePool_);
    }
    epollServer(int port) : port(port) {
        pool = new ThreadPool(new ResponseThreadPool());
    }

    int init() {
        memset(&service_addr, 0, sizeof(service_addr));
        service_addr.sin_family = AF_INET;
        service_addr.sin_port = htons(port);
        service_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        cout<<listenfd<<endl;
        int err_log = ::bind(listenfd, (struct sockaddr*)&service_addr, sizeof(service_addr));
        if (err_log != 0) {
            perror("bind");
            exit(EXIT_FAILURE);
        }

        epollfd = epoll_create1(0);
        if (epollfd == -1) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = listenfd;

        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
            perror("epoll_ctl: listen_sock");
            exit(EXIT_FAILURE);
        }

        return listenfd;
    }

    int mylisten() {
        return listen(listenfd, 10);
    }

    int run() {
        struct epoll_event events[MAX_EVENTS];

        while (1) {
            int num_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
            if (num_fds == -1) {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < num_fds; ++i) {
                
                if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))) {
                    fdStatus::get_instance().closedata(events[i].data.fd);
                    fprintf(stderr, "连接已关闭");
                    close(events[i].data.fd);
                    continue;
                } else if (listenfd == events[i].data.fd) {
                    cout << "监听到请求" << endl;
                    struct sockaddr_in client_addr;
                    socklen_t len = sizeof(client_addr);
                    int sockfd = accept(listenfd, (struct sockaddr*)&client_addr, &len);
                    cout<<sockfd<<endl;
                    if (sockfd == -1) {
                        perror("accept");
                        continue;
                    }
                    struct epoll_event event;
                    event.events = EPOLLIN | EPOLLET;
                    event.data.fd = sockfd;

                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
                        perror("epoll_ctl: conn_sock");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    cout<<listenfd<<endl;
                    cout<<events[i].data.fd<<endl;
                    pool->addTask(events[i].data.fd);
                    fdStatus::get_instance().setdata(events[i].data.fd, true);
                }
            }
        }
    }

    void runall() {
        cout << "初始化" << endl;
        cout << init() << endl;
        cout << "正在监听："<< port << endl;
        cout << mylisten() << endl;
        cout << ntohs(service_addr.sin_port) << " " << ntohl(service_addr.sin_addr.s_addr) << endl;
        cout << "开始执行" << endl;
        run();
        close(epollfd);
        close(listenfd);
    }
};
