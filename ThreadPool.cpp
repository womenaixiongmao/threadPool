//
// Created by cuizhuofu on 2021/4/25.
//
#include "ThreadPool.h"

ThreadPool::ThreadPool(int threadNum)
{
    if (threadNum < 0) {
        return;
    }
    for (int index = 0; index < threadNum; ++index) {
        m_threads.emplace_back([this](){
            while (true) {
                {
                    unique_lock<mutex> lock(m_queueMutex);
                    m_condition.wait(lock, [this]{return m_stop || !m_tasks.empty()});
                    if (m_stop && m_tasks.empty()) {
                        return;
                    }
                    auto task = m_tasks.front();
                    m_tasks.pop();
                    task();
                }
            }
        });
    }
}
ThreadPool::~ThreadPool()
{
    Stop();
}
void ThreadPool::Stop()
{
    m_stop.store(true);
    for (auto& work : m_threads) {
        work.join();
    }
    m_condition.notify_all();
}