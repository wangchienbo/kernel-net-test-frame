#include <vector>
#include <thread>
#include <mutex>

class ThreadGroup {
private:
    std::vector<std::thread> threads;
    std::mutex mutex;

public:
    // 禁止拷贝构造和赋值操作
    ThreadGroup(const ThreadGroup&) = delete;
    ThreadGroup& operator=(const ThreadGroup&) = delete;
    static ThreadGroup& get_instance(){
        static ThreadGroup instance;
        return instance;
    }
    ThreadGroup() = default;
    ~ThreadGroup() {
        wait();
    }


    // 添加线程到组中
    template<typename Callable, typename... Args>
    void addThread(Callable&& func, Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex);
        threads.emplace_back(std::forward<Callable>(func), std::forward<Args>(args)...);
    }

    // 等待所有线程完成
    void wait() {
        for (std::thread& thread : threads) {
            cout<<"join thread"<<endl;
            if (thread.joinable()) {
                thread.join();
            }
        }
        threads.clear(); // 清空线程列表，以便重用ThreadGroup
    }
};