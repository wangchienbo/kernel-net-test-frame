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
#include "response.h"
using namespace std;
// Define the maximum size of the thread pool
const int MAX_POOL_SIZE = 8;

// Define the size of the message queue
const int MESSAGE_QUEUE_SIZE = 100;

// Define the size of the result pool
const int RESULT_POOL_SIZE = 100;

// Define the byte array size
const int BYTE_ARRAY_SIZE = 1024;

// Define the thread pool class
class ResponseThreadPool{
    public:
        MessageQueue& taskQueue;
        ResultQueue& resultQueue;
        Response result;
        ResponseThreadPool() : stop(false), taskQueue(MessageQueue::Instance()), resultQueue(ResultQueue::Instance()){
            // Create worker threads
            // resultQueue.respool=this;
            for (int i = 0; i < MAX_POOL_SIZE; ++i) {
                workers.emplace_back([this] {
                    while (true) {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !resultQueue.empty(); });
                        if (stop) {
                            return;
                        }
                        auto res = resultQueue.getResult();
                        condition.notify_one();
                        processResult(res);
                        sendRes(res.first);
                    }
                });
            }
        }
        virtual void processResult(pair<int,std::vector<uint8_t>> res){
            // process the result
            std::string str((res.second).begin(),(res.second).end());
            result.msg = str;
        }
        virtual void sendRes(int fd){
            string res="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 11\r\n\r\n";
            res+="\n";
            res+=result.msg+"#";
            cout<<"返回"<<res<<endl;
            send(fd, res.c_str(), res.size(), 0);
        }
    void addResult(pair<int,std::vector<uint8_t>> result) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        resultQueue.addResult(result);
        condition.notify_one();
    }
    void notify() {
        condition.notify_one();
    }


    ~ResponseThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
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