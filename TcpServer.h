/**
 * TcpServer.h
 * tcp服务器，封装套接字
 * yui
 * 2026-03-16
 * 2026-03-18加入线程池
 */

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <iostream>
#include <chrono>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include "Logger.h"
#include "ThreadPool.h"
#include "Buffer.h"


#define BUFF_SIZE 1024

/**
 * Tcp服务器
 * 封装套接字
 * 初始化TcpServer对象时完成套接字的创建、绑定和监听
 * 单例模式
 */
class TcpServer
{
public:
    enum class TCP_STATUS
    {
        OK = 0,
        SOCK_ERR,
        BIND_ERR,
        LINSTIN_ERR,
        ACCEPT_ERR
    };

    TcpServer(const TcpServer &tcp_server) = delete;
    TcpServer(const TcpServer &&tcp_server) = delete;
    TcpServer &operator=(const TcpServer &tcp_server) = delete;
    TcpServer &operator=(const TcpServer &&tcp_server) = delete;

    static TcpServer &getInstance()
    {
        static TcpServer instance;
        return instance;
    }

    //创建套接字
    TCP_STATUS createSocket();

    //阻塞等待客户端连接
    void loop();

    //接收消息
    bool recvMsg(const int connect_fd,std::string& msg);
    bool sendMsg(const int connect_fd,const std::string& msg);

    void init(const int port,const int thread_num)
    {
        port_ = port;
        thread_pool_ = std::make_unique<ThreadPool>(thread_num);
        //启动线程池
        thread_pool_->start();
    }

private:
    TcpServer()
        : port_(0),
          listen_sock_(-1)
    {
    }
    ~TcpServer() {}
    void handleClient(int connect_fd)
    {
        std::string msg;
        if(recvMsg(connect_fd,msg))
        {
            LOG_DEBUG("客户端请求处理完成并关闭连接fd:%d",connect_fd);
        }
    }

private:
    int port_;
    int listen_sock_;
    std::unique_ptr<ThreadPool> thread_pool_;
};

#endif