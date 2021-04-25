//
// Created by cuizhuofu on 2021/4/25.
//

#ifndef UNTITLED_THREADPOOL_H
#define UNTITLED_THREADPOOL_H

#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>

using namespace std;

class ThreadPool {
public:
    ThreadPool(int threadNum = 0);
    ~ThreadPool();
    template<typename F, typename... Args>
    void AddTaskToPool(F&& f, Args&&... args);
    void Stop();
private:
    vector<thread> m_threads;
    queue<function<void()>> m_tasks;
    atomic<bool> m_stop;
    condition_variable m_condition;
    mutex m_queueMutex;
};
template<typename F, typename... Args>
void ThreadPool::AddTaskToPool(F&& f, Args&&... args)
{
    auto packaged = bind(forward<F>(f), forward<Args>(args)...);
    {
        unique_lock<mutex> lock(m_queueMutex);
        if (m_stop) {
            return;
        }
        m_tasks.emplace([packaged](){
            (*packaged)();
        });
    }
    m_condition.notify_one();
}

#endif //UNTITLED_THREADPOOL_H
