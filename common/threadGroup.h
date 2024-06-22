#include <vector>
#include <thread>
#include <mutex>

class ThreadGroup {
private:
    std::vector<std::thread> threads;
    map<string, vector<int>> threadMap;
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
    void addThread(string id, Callable&& func, Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex);
        threads.emplace_back(std::forward<Callable>(func), std::forward<Args>(args)...);
        if(threadMap.find(id)==threadMap.end()){
            threadMap[id]=vector<int>();
        }
        threadMap[id].push_back(threads.size()-1);
    }
    void wait(string id) {
        std::lock_guard<std::mutex> lock(mutex);
        for(int i=0;i<threadMap[id].size();i++){
            if (threads[threadMap[id][i]].joinable()) {
                threads[threadMap[id][i]].join();
            }
        }
        threadMap[id].clear();
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