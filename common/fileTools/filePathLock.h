#include <iostream>
#include <map>
#include <mutex>
#include <memory>
#include "../rwLock.h"
using namespace std;
class FilePathLock {
public:
    std::map<std::string, std::shared_ptr<rwLock>> pathLocks;
    mutex mapLock;
    FilePathLock() {
        pathLocks = std::map<std::string, std::shared_ptr<rwLock>>();
    }
    static FilePathLock &get_instance() {
        static FilePathLock instance;
        return instance;
    }
    void init(string path) {
        std::lock_guard<std::mutex> lock(mapLock);
        if (pathLocks.find(path) == pathLocks.end()) {
            pathLocks[path] = std::make_shared<rwLock>();
        }
    }
    void lock(string path) {
        init(path);
        pathLocks[path]->lockWrite();
    }
    void unlock(string path) {
        pathLocks[path]->unlockWrite();
    }
    void lockRead(string path) {
        init(path);
        pathLocks[path]->lockRead();
    }
    void unlockRead(string path) {
        pathLocks[path]->unlockRead();
    }
};