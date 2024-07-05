#include <iostream>
#include <mutex>
class rwLock {
public:
    int readerCount;
    std::mutex rLock;
    std::mutex wLock;
    rwLock() {
        readerCount = 0;
    }
    void lockRead() {
        std::lock_guard<std::mutex> lock(rLock);
        if (readerCount == 0) {
            lockWrite();
        }
        readerCount++;
    }
    void unlockRead() {
        std::lock_guard<std::mutex> lock(rLock);
        readerCount--;
        if (readerCount == 0) {
            unlockWrite();
        }
    }
    void lockWrite() {
        wLock.lock();
    }
    void unlockWrite() {
        if (wLock.try_lock()) {
            std::cout << "unlock write" << std::endl;
            wLock.unlock();
            return;
        }
        wLock.unlock();
    }
};