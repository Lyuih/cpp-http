/**
 * TcpServer.h
 * tcp服务器，封装套接字
 * yui
 * 2026-03-16
 * 2026-03-17基础功能完成
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

    void init(const int port)
    {
        port_ = port;
    }

private:
    TcpServer()
        : port_(0),
          listen_sock_(-1)
    {
    }
    ~TcpServer() {}

private:
    int port_;
    int listen_sock_;
};

#endif