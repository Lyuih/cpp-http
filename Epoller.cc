#include "Epoller.h"
// 添加/修改/删除监听
bool Epoller::addFd(int fd, uint32_t events)
{
    if (fd < 0)
        return false;
    struct epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    // 注册fd到监视名单中
    return 0 == epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev);
}
bool Epoller::modFd(int fd, uint32_t events)
{
    if (fd < 0)
        return false;
    struct epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev);
}
bool Epoller::delFd(int fd)
{
    if (fd < 0)
        return false;
    return 0 == epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr);
}

// 等待事件发生，返回活跃的事件数量
int Epoller::wait(int timeoutMs)
{
    return epoll_wait(epollFd_, events_.data(), static_cast<int>(events_.size()), timeoutMs);
}
// 获取发生事件的第i个fd
int Epoller::getEventFd(size_t i) const
{
    return events_[i].data.fd;
}
// 获取发生事件的第i个事件类型
uint32_t Epoller::getEvents(size_t i) const
{
    return events_[i].events;
}