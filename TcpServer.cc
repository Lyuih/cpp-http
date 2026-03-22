#include "TcpServer.h"

TcpServer::TCP_STATUS TcpServer::createSocket()
{
    int ret = 0;
    // 1.创建套接字
    ret = socket(AF_INET, SOCK_STREAM, 0);
    if (ret == -1)
    {
        LOG_FATAL("套接字创建失败");
        return TCP_STATUS::SOCK_ERR;
    }
    LOG_DEBUG("创建套接字成功, listen_sock_=%d", ret);
    listen_sock_ = ret;

    // 设置端口复用
    int opt = 1;
    setsockopt(listen_sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2.绑定端口信息
    struct sockaddr_in addr;
    ::bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = ::htons(port_);
    ret = ::bind(listen_sock_, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        LOG_FATAL("套接字绑定失败");
        return TCP_STATUS::BIND_ERR;
    }
    LOG_DEBUG("绑定套接字成功");
    // 3.开始监听是否有客户端连接
    ret = ::listen(listen_sock_, 10);
    if (ret == -1)
    {
        LOG_FATAL("监听客户端失败");
        return TCP_STATUS::LINSTIN_ERR;
    }
    LOG_DEBUG("监听套接字成功");
    return TCP_STATUS::OK;
}

void TcpServer::loop()
{
    LOG_DEBUG("开始等待客户端连接");
    epoller_.addFd(listen_sock_, EPOLLIN); 
    for (;;)
    {
        //一直等
        int count = epoller_.wait(-1);
        for(int i = 0;i<count;++i)
        {
            int fd = epoller_.getEventFd(i);
            if(fd == listen_sock_)
            {
                handleAccept();
            }
            else
            {
                // epoller_.delFd(fd);
                thread_pool_->append(std::bind(&TcpServer::handleClient, this, fd));
            }
        }
        // LOG_INFO("客户端连接建立成功，准备分发给线程池");
        // std::string msg;
        // // recvMsg(connect_fd,msg);
        // thread_pool_->append(std::bind();
    }
}

bool TcpServer::recvMsg(const int connect_fd, std::string &msg)
{
    // char buf[BUFF_SIZE];
    Buffer read_buf;

    // ssize_t n = ::recv(connect_fd, buf, BUFF_SIZE, 0);
    int err;
    int n = read_buf.readFd(connect_fd, &err);
    if (n == -1)
    {
        LOG_ERROR("读取客户端信息失败");
        return false;
    }
    else if (n == 0)
    {
        LOG_INFO("客户端连接关闭");
        // std::cout<<msg<<std::endl;
    }
    else
    {
        ;
    }

    return true;
}

bool TcpServer::sendMsg(const int connect_fd, const std::string &msg)
{
    ssize_t n = ::send(connect_fd, msg.c_str(), msg.size(), 0);
    if (n == -1)
    {
        LOG_ERROR("消息发送失败");
        return false;
    }
    LOG_DEBUG("消息发送成功");
    return true;
}

void TcpServer::handleAccept()
{
    struct sockaddr_in addr;
    ::bzero(&addr, sizeof(addr));
    socklen_t len = sizeof(addr);
    int ret = ::accept(listen_sock_, (struct sockaddr *)&addr, &len);
    if (ret == -1)
    {
        LOG_ERROR("客户端连接错误：%s. listen_sock_=%d",strerror(errno), listen_sock_);
        // std::this_thread(sle)
        ::sleep(1); // 避免刷屏
        return;
    }
    int connect_fd = ret;
    //将新fd注册到epoll监听名单中
    // 监听可读事件(EPOLLIN)和对端断开事件(EPOLLRDHUP)
    //使用LT模式
    epoller_.addFd(connect_fd,EPOLLIN | EPOLLRDHUP|EPOLLONESHOT);
    LOG_INFO("新连接建立 fd : %d",connect_fd);
}