#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
class MessageQueue {
public:
    // ThreadPool* msgpool;
    void push(int fd) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(fd);
        lock.unlock();
        condition_.notify_one();
        // msgpool->notify();
    }

    int pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty()) {
            condition_.wait(lock);
        }
        int fd = queue_.front();
        queue_.pop();
        return fd;
    }
    bool empty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    static MessageQueue &Instance() {
        static MessageQueue instance;
        return instance;
    }

private:
    std::queue<int> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
};