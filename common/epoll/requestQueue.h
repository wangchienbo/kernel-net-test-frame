#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "head.h"
using namespace std;

class ResultQueue {
public:
    // ResponseThreadPool *respool;
    void addResult(pair<int,std::vector<uint8_t>> result) {
        std::lock_guard<std::mutex> lock(mutex_);
        results_.push(result);
        condition_.notify_one();
        // respool->notify();
    }

    pair<int,std::vector<uint8_t>> getResult() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !results_.empty(); });
        pair<int,std::vector<uint8_t>> result = results_.front();
        results_.pop();
        return result;
    }
    bool empty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return results_.empty();
    }
    static ResultQueue& Instance() {
        static ResultQueue instance;
        return instance;
    }

private:
    std::queue<pair<int,std::vector<uint8_t>>> results_;
    std::mutex mutex_;
    std::condition_variable condition_;
};