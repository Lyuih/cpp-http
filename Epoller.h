/**
 * @file    Epoller.h
 * @brief   封装epoll
 * @author  yui
 * @date    2026-03-20
 *
 * @history
 * 2026-03-20  初次创建
 */

#ifndef EPOLLER_H
#define EPOLLER_H
#include <vector>
#include <unistd.h>
#include <sys/epoll.h>

class Epoller
{
public:
    explicit Epoller(int maxEvents = 1024)
    :epollFd_(epoll_create(1024)),//参加epoll句柄
    events_(maxEvents)
    {}
    ~Epoller()
    {
        if(epollFd_!= -1)
        {
            close(epollFd_);
        }
    }
    // 添加/修改/删除监听
    bool addFd(int fd, uint32_t events);
    bool modFd(int fd, uint32_t events);
    bool delFd(int fd);

    // 等待事件发生，返回活跃的事件数量
    int wait(int timeoutMs = -1);
    // 获取发生事件的第i个fd
    int getEventFd(size_t i) const;
    // 获取发生事件的第i个事件类型
    uint32_t getEvents(size_t i) const;

private:
    int epollFd_;
    // 存储活跃事件的数组
    std::vector<struct epoll_event> events_;
};
#endif
