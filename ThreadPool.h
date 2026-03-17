/**
 * ThreadPool.h
 * 线程池
 * yui
 * 2026-03-17
 * 2026-03-17初次创建
 */
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <queue>
#include <functional>
#include <condition_variable>
#include "Logger.h"

#define NUM 10

/**
 * 线程池
 */
class ThreadPool
{
public:
    using func_task = std::function<void()>;
    ThreadPool(const int num = NUM);
    // 禁止拷贝
    ThreadPool(const ThreadPool &thread_pool) = delete;
    ThreadPool &operator=(const ThreadPool &thread_pool) = delete;

    ~ThreadPool();
    // 启动线程池
    void start();
    // 关闭线程池
    void shutdown();
    // 添加任务
    void append(const func_task &task);

private:
    bool isEmpty()
    {
        return tasks_.empty();
    }
    // 启动线程处理任务
    void handle(std::string name);

private:
    // 线程数量
    int num_;
    // 线程池运行状态
    bool is_running_;
    // 存储线程的数组
    std::vector<std::thread> threads_;
    // 存储任务的队列
    std::queue<func_task> tasks_;
    // 互斥锁
    std::mutex mutex_;
    // 同步锁
    std::condition_variable cond_;
};

#endif