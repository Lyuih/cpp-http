/**
 * @file HttpServer.h
 * @brief Http服务器，处理tcp连接，实现http请求和响应
 * @author yui
 * @date 2026-03-16
 *
 * @history
 * 2026-03-16
 * 2026-03-16初次创建
 * 2026-03-20实现http服务器主要功能
 */

#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include "TcpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

/**
 * http服务器
 * 处理tcp连接
 * 负责存储http请求和响应
 */
// 默认端口号
#define DEFAULT_PORT 8888
#define THREAD_NUM 10

class HttpServer
{
public:
    HttpServer(const int port = DEFAULT_PORT, const int thread_num = THREAD_NUM);
    // 循环监听获取新连接
    void loop();

    // 核心业务处理入口
    void onMessage(int fd);

private:
    // 根据请求路径分发逻辑
    void handleRequest(int fd, HttpRequest &req);

private:
    int port_;
    int thread_num_;
};

#endif