#pragma once
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "messageQueue.h"
#include <unistd.h>
#include <vector>
#include "requestQueue.h"
#include "responseThreadPool.h"

using namespace std;

std::vector<uint8_t> readFromFd(int fd) {
    std::vector<uint8_t> buffer;
    uint8_t byte;
    int f = read(fd, &byte, 1);
    while (f > 0) {
        if (byte == '#') {
            break;
        }
        buffer.push_back(byte);
        f = read(fd, &byte, 1);
    }
    cout << f << endl;
    if (f == 0) {
        fdStatus::get_instance().closedata(fd);
    }
    cout << "read from fd,byte size : " << buffer.size() << endl;
    return buffer;
}

// Define the thread pool class
class ThreadPool {
public:
    MessageQueue &taskQueue;
    ResponseThreadPool *respool;
    ThreadPool(ResponseThreadPool *respool) :
        stop(false), taskQueue(MessageQueue::Instance()) {
        this->respool = respool;
        // Create worker threads
        // MessageQueue::Instance().msgpool=this;
        for (int i = 0; i < MAX_POOL_SIZE; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    condition.wait(lock, [this] { return stop || !MessageQueue::Instance().empty(); });
                    if (stop) {
                        return;
                    }
                    int fd = MessageQueue::Instance().pop();
                    condition.notify_one();
                    auto res = make_pair(fd, readFromFd(fd));
                    this->respool->addResult(res);
                }
            });
        }
    }
    void notify() {
        condition.notify_one();
    }
    void addTask(int fd) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        taskQueue.push(fd);
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers) {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};