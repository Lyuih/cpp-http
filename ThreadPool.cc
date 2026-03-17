#include "ThreadPool.h"

ThreadPool::ThreadPool(const int num)
    : num_(num),
      is_running_(false)
{
}

ThreadPool::~ThreadPool()
{
    if (is_running_)
    {
        shutdown();
    }
}

void ThreadPool::start()
{
    is_running_ = true;
    for (int i = 0; i < num_; ++i)
    {
        threads_.push_back(std::thread(&ThreadPool::handle, this, "thread-" + std::to_string(i)));
    }
    LOG_DEBUG("启动线程池成功");
}

void ThreadPool::shutdown()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        is_running_ = false;
    }
    // 唤醒所有线程
    cond_.notify_all();
    for (auto &th : threads_)
    {
        if (th.joinable())
            th.join();
    }
}

void ThreadPool::append(const func_task &task)
{
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.push(task);
    cond_.notify_one();
}

void ThreadPool::handle(std::string name)
{
    while (is_running_)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]()
                   { return !is_running_ || !isEmpty(); });
        if (is_running_ == false)
        {
            break;
        }
        LOG_DEBUG("%s 开始执行任务", name.c_str());
        func_task task = tasks_.front();
        tasks_.pop();
        lock.unlock();

        task();
    }
    LOG_DEBUG("%s停止工作", name.c_str());
}